//
// Created by riyafa on 12/6/16.
//
/* File:     pthread_linked_list_readwritelock.c
 *
 * Purpose:  Implement a multi-threaded sorted linked list of
 *           ints with ops insert, print, member, delete, free list.
 *           This version uses read-write locks
 *
 * Compile:  gcc -g -Wall pthread_linked_list_readwritelock.c -o pthread_linked_list_readwritelock -lpthread -lm
 * Usage:    ./pthread_linked_list_readwritelock
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
#include <math.h>
#include <sys/time.h>

const int MAX_KEY = 65535;

struct list_node_s {
    int data;
    struct list_node_s *next;
};
struct list_node_s *head = NULL;
int thread_count, total_ops;
unsigned int seed;
double search_percent, insert_percent, delete_percent;
pthread_rwlock_t rwlock;
pthread_mutex_t count_mutex;
int member_count = 0, insert_count = 0, delete_count = 0, max_member_number, max_insert_number, max_delete_number;

int Insert(int value) {
    struct list_node_s *curr = head;
    struct list_node_s *pred = NULL;
    struct list_node_s *temp;
    int rv = 1;

    while (curr != NULL && curr->data < value) {
        pred = curr;
        curr = curr->next;
    }

    if (curr == NULL || curr->data > value) {
        temp = malloc(sizeof(struct list_node_s));
        temp->data = value;
        temp->next = curr;
        if (pred == NULL)
            head = temp;
        else
            pred->next = temp;
    } else { /* value in list */
        rv = 0;
    }

    return rv;
}

int is_empty(void) {
    if (head == NULL)
        return 1;
    else
        return 0;
}

void free_list(void) {
    struct list_node_s *current;
    struct list_node_s *following;

    if (is_empty()) return;
    current = head;
    following = current->next;
    while (following != NULL) {
        free(current);
        current = following;
        following = current->next;
    }
    free(current);
}

int Member(int value) {
    struct list_node_s *temp;

    temp = head;
    while (temp != NULL && temp->data < value)
        temp = temp->next;

    if (temp == NULL || temp->data > value) {
        return 0;
    } else {
        return 1;
    }
}

int Delete(int value) {
    struct list_node_s *curr = head;
    struct list_node_s *pred = NULL;
    int rv = 1;

    /* Find value */
    while (curr != NULL && curr->data < value) {
        pred = curr;
        curr = curr->next;
    }

    if (curr != NULL && curr->data == value) {
        if (pred == NULL) { /* first element in list */
            head = curr->next;
            free(curr);
        } else {
            pred->next = curr->next;
            free(curr);
        }
    } else { /* Not in list */
        rv = 0;
    }

    return rv;
}

void *Thread_operation(void *rank) {
    long my_rank = (long) rank;
    unsigned int seedp = seed + my_rank;
    int i, val;
    int my_member_count = 0, my_insert_count = 0, my_delete_count = 0;
    int ops_per_thread = total_ops / thread_count;
    int thread_max_member_number = max_member_number / thread_count;
    int thread_max_insert_number = max_insert_number / thread_count;
    for (i = 0; i < ops_per_thread; i++) {
        val = rand_r(&seedp) % MAX_KEY;
        if (my_member_count < thread_max_member_number) {
            pthread_rwlock_rdlock(&rwlock);
            Member(val);
            pthread_rwlock_unlock(&rwlock);
            my_member_count++;
        } else if (my_insert_count < thread_max_insert_number) {
            pthread_rwlock_wrlock(&rwlock);
            Insert(val);
            pthread_rwlock_unlock(&rwlock);
            my_insert_count++;
        } else { /* delete */
            pthread_rwlock_wrlock(&rwlock);
            Delete(val);
            pthread_rwlock_unlock(&rwlock);
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

double threadFunction(int iterations, int inserts_in_main) {
    head = NULL;
    for (int i = 0; i < inserts_in_main; i++) {
        int key = rand_r(&seed) % MAX_KEY;
        if (Insert(key)) {
            i++;
        }
    }
    printf("Inserted %d keys in empty list\n", inserts_in_main);
    pthread_t *thread_handles = malloc(thread_count * sizeof(pthread_t));

    double begin = getCurrentTime();
    max_member_number = (int) total_ops * search_percent;
    max_insert_number = (int) total_ops * insert_percent;
    max_delete_number = (int) total_ops * delete_percent;
    for (long i = 0; i < thread_count; i++) {
        pthread_create(&thread_handles[i], NULL, Thread_operation, (void *) i);
    }
    for (int i = 0; i < thread_count; i++) {
        pthread_join(thread_handles[i], NULL);
    }
    double end = getCurrentTime();
    double time = end - begin;
    printf("Elapsed time = %e seconds\n", time);
    printf("Total operations = %d\n", total_ops);
    printf("member operations = %d\n", member_count);
    printf("insert operations = %d\n", insert_count);
    printf("delete operations = %d\n", delete_count);
    free_list();
    free(thread_handles);
    return time;
}

int main() {
    int iterations, inserts_in_main;
    printf("Enter number of samples: ");
    scanf("%d", &iterations);
    printf("Enter number of threads: ");
    scanf("%d", &thread_count);
    printf("Enter number of keys to be inserted in the main thread (n)?\n");
    scanf("%d", &inserts_in_main);
    printf("Enter total operations to be executed (m)?\n");
    scanf("%d", &total_ops);
    printf("Enter percent of operations that should be searches? (between 0 and 1)\n");
    scanf("%lf", &search_percent);
    printf("Enter percent of operations that should be inserts? (between 0 and 1)\n");
    scanf("%lf", &insert_percent);
    delete_percent = 1.0 - (search_percent + insert_percent);
    seed = time(NULL);
    pthread_mutex_init(&count_mutex, NULL);
    pthread_rwlock_init(&rwlock, NULL);
    double timespent[iterations];
    double sum = 0, sum_var = 0, average = 0, std_deviation = 0, variance = 0;
    for (int i = 0; i < iterations; i++) {
        member_count = 0;
        insert_count = 0;
        delete_count = 0;
        printf("\n\n---------------Sample %d---------------\n", i + 1);
        timespent[i] = threadFunction(iterations, inserts_in_main);
        sum += timespent[i];
    }
    average = sum / (float) iterations;
    /*  Compute  variance  and standard deviation  */
    for (int i = 0; i < iterations; i++) {
        sum_var = sum_var + pow((timespent[i] - average), 2);
    }
    variance = sum_var / (float) (iterations - 1);
    std_deviation = sqrt(variance);

    printf("\n\nAverage: %e seconds \nStandard deviation: %e seconds\n", average, std_deviation);
    pthread_rwlock_destroy(&rwlock);
    pthread_mutex_destroy(&count_mutex);
    return 0;
}