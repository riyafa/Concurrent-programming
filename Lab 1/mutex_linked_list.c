//
// Created by malith on 12/10/16.
//
/* File:     linked_list_serial.c
 *
 * Purpose:  Implement a serial sorted linked list of
 *           ints with ops insert, print, member, delete, free list.
 *           This version uses mutex locks
 *
 * Compile:  gcc -g -Wall -std=gnu99 mutex_linked_list.c -o mutex_linked_list -lpthread -lm
 * Usage:    ./mutex_linked_list 
 * Input:    total number of samples
 *           total number of keys inserted by main thread
 *           total number of operations
 *           percent of operations that are search, insert (remainder are delete)
 * Output:   Elapsed time to carry out the operations for each sample
 *           The average and standard deviation of the times for the given number of samples
 *
 * Notes:
 *    1.  Repeated values are not allowed in the list
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>

int MAX_NUMBER = 65535;
int max_m_operations = 200;
int thread_count = 8;
int initial_length = 10;
int seed =18;

double member_fraction = 0.80;
double insert_fraction = 0.10;
double delete_fraction = 0.10;

struct node** head_node;

pthread_mutex_t list_mutex;
pthread_mutex_t count_mutex;

int max_member_number=0;
int max_insert_number=0;
int max_delete_number=0;

int member_count = 0, insert_count = 0, delete_count = 0;

struct node {
  int data;
  struct node *next;
};

int Member(int value, struct node* head){
    struct node* curr=head;

    while(curr!=NULL && curr->data < value)
        curr = curr->next;

    if(curr == NULL || curr->data > value){
        return 0;
    }else{
        return 1;
    }
}

int Insert(int value, struct node** head){
    struct node* curr = *head;
    struct node* prev = NULL;
    struct node* temp;

    while(curr!=NULL && curr-> data < value){
        prev=curr;
        curr=curr->next;
    }

    if(curr==NULL || curr->data > value){
        temp = malloc(sizeof(struct node));
        temp->data = value;
        temp->next = curr;

        if(prev==NULL){
            *head=temp;
        }
        else{
            prev->next=temp;
        }
        return 1;
    }else{
        return 0;  /*value is already in the list*/
    }
}

int Delete(int value, struct node** head){
    struct node* curr= *head;
    struct node* prev= NULL;

    while(curr!=NULL && curr-> data < value){
        prev=curr;
        curr=curr->next;
    }

    if(curr!=NULL && curr->data == value){
        if(prev == NULL){ /*Deleting the first node in the list*/
            *head = curr->next;
            free(curr);
        }else{
            prev->next = curr->next;
            free(curr);
        }
        return 1;
    }else{
        return 0;  /*Value is not in the list*/
    }
}

void Populate_list(int seed, struct node** head, int n){
    srand(seed);
    *head = malloc(sizeof(struct node));
    (*head)->data = rand() % (MAX_NUMBER + 1);
    int result = 0;

    for(int i=0;i<n-1;i++){
        result = Insert(rand() % (MAX_NUMBER + 1),head);
        if(!result){                                    //If the number is in the list already, generate another number
            i=i-1;
        }
    }
}

void* Thread_operation(void* rank){
    long my_rank = (long) rank;
    unsigned int seedp = seed + my_rank;
    int i, val;
    int my_member_count = 0, my_insert_count = 0, my_delete_count = 0;
    int ops_per_thread = max_m_operations / thread_count;
    int thread_max_member_number = max_member_number / thread_count;
    int thread_max_insert_number = max_insert_number / thread_count;
    int thread_max_delete_number = max_delete_number / thread_count;
    for (i = 0; i < ops_per_thread; i++) {
        val = rand_r(&seedp) % MAX_NUMBER;
        if (my_member_count < thread_max_member_number) {
            pthread_mutex_lock(&list_mutex);
            Member(val, *head_node);
            pthread_mutex_unlock(&list_mutex);
            my_member_count++;
        } else if (my_insert_count < thread_max_insert_number) {
            pthread_mutex_lock(&list_mutex);
            Insert(val, head_node);
            pthread_mutex_unlock(&list_mutex);
            my_insert_count++;
        } else if(my_delete_count < thread_max_delete_number) { /* delete */
            pthread_mutex_lock(&list_mutex);
            Delete(val, head_node);
            pthread_mutex_unlock(&list_mutex);
            my_delete_count++;
        }
    }

    pthread_mutex_lock(&count_mutex);
    member_count += my_member_count;
    insert_count += my_insert_count;
    delete_count += my_delete_count;
    pthread_mutex_unlock(&count_mutex);
    return NULL;
}

double getCurrentTime() {
    struct timeval te;
    gettimeofday(&te, NULL);
    return te.tv_sec + te.tv_usec / 1000000.0;
}

int main()
{
    head_node = malloc(sizeof(struct node*));

    //initialize mutex
    pthread_mutex_init(&list_mutex, NULL);
    pthread_mutex_init(&count_mutex, NULL);

    int iterations=0;
    initial_length=1000;
    max_m_operations=10000;
    member_fraction=0.99;
    insert_fraction=0.005;
    delete_fraction=0.005;
    thread_count=4;

    printf("Enter number of samples: ");
    scanf("%d",&iterations);
    printf("Enter n: ");
    scanf("%d",&initial_length);
    printf("Enter m: ");
    scanf("%d",&max_m_operations);
    printf("Enter member_fraction: ");
    scanf("%lf",&member_fraction);
    printf("Enter insert_fraction: ");
    scanf("%lf",&insert_fraction);
    printf("Enter delete_fraction: ");
    scanf("%lf",&delete_fraction);
    printf("Enter thread count: ");
    scanf("%d",&thread_count);

    double timespent[iterations];
    for(seed=10; seed<11+iterations; seed++){
        Populate_list(seed, head_node, initial_length);

        max_member_number = (int)max_m_operations*member_fraction;
        max_insert_number = (int)max_m_operations*insert_fraction;
        max_delete_number = (int)max_m_operations*delete_fraction;

        double begin = getCurrentTime();

        pthread_t threads[thread_count];
        long thread;
        for(thread=0;thread<thread_count;thread++){
            pthread_create(&threads[thread], NULL, Thread_operation, (void*) thread);
        }

        for(thread=0; thread<thread_count;thread++){
            pthread_join(threads[thread],NULL);
        }

        double end = getCurrentTime();
        double time_spent = end - begin;
        timespent[seed-10]=time_spent;
        //printf("Seed: %d, Time taken: %f seconds",seed,time_spent);
        printf("Percentage Complete: %.0f %%",((seed-10)*100)/(float)iterations);
        printf("\r");
    }

    /** Calculate average and Std **/
    double sum=0, sum_var=0, average=0, std_deviation=0, variance=0;
    for (int i = 0; i < iterations; i++)
    {
        sum += timespent[i];
    }
    average = sum / (float)iterations;
    /*  Compute  variance  and standard deviation  */
    for (int i = 0; i < iterations; i++)
    {
        sum_var = sum_var + pow((timespent[i] - average), 2);
    }
    variance = sum_var / (float)(iterations-1);
    std_deviation = sqrt(variance);

    printf("Average: %f, std_deviation: %f\n",average,std_deviation);

    pthread_mutex_destroy(&list_mutex);
    pthread_mutex_destroy(&count_mutex);
    return 0;
}
