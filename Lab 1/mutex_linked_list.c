#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
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

int max_member_number=0;
int max_insert_number=0;
int max_delete_number=0;

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
    /* Define local limits for the threads */
    int thread_max_member_number = max_member_number/thread_count;
    int thread_max_insert_number = max_insert_number/thread_count;
    int thread_max_delete_number = max_delete_number/thread_count;
    int thread_member_number=0;
    int thread_insert_number=0;
    int thread_delete_number=0;
    srand(seed);
    int operation;
    int random_number;

    while(thread_member_number<thread_max_member_number || thread_insert_number<thread_max_insert_number || thread_delete_number<thread_max_delete_number){
        random_number = rand() % (MAX_NUMBER+1);
        operation = random_number % 3;   //Choose the operation randomly

        if(operation==0 && thread_member_number < thread_max_member_number){
                thread_member_number++;
                pthread_mutex_lock(&list_mutex);
                Member(random_number, *head_node);
                pthread_mutex_unlock(&list_mutex);
        }
        else if(operation==1 && thread_insert_number < thread_max_insert_number){
                thread_insert_number++;
                pthread_mutex_lock(&list_mutex);
                Insert(random_number, head_node);
                pthread_mutex_unlock(&list_mutex);
        }
        else if(operation==2 && thread_delete_number < thread_max_delete_number){
                thread_delete_number++;
                pthread_mutex_lock(&list_mutex);
                Delete(random_number, head_node);
                pthread_mutex_unlock(&list_mutex);
        }

    }
    return NULL;
}


int main()
{
    head_node = malloc(sizeof(struct node*));

    //initialize mutex
    pthread_mutex_init(&list_mutex, NULL);

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

        clock_t begin = clock();

        pthread_t threads[thread_count];
        long thread;
        for(thread=0;thread<thread_count;thread++){
            pthread_create(&threads[thread], NULL, Thread_operation, (void*) thread);
        }

        for(thread=0; thread<thread_count;thread++){
            pthread_join(threads[thread],NULL);
        }

        clock_t end = clock();
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
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

    return 0;
}
