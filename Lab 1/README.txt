File:     linked_list_serial.c
 
Purpose:  Implement a serial sorted linked list of
            ints with operations insert, member, delete, free list.
 
Compile:  gcc -g -Wall -std=gnu99 linked_list_serial.c -o linked_list_serial -lm
Usage:    ./linked_list_serial
Input:    total number of samples
            total number of keys inserted to initiate the list
            total number of operations
            percent of operations that are search, insert (remainder are delete)
Output:   Elapsed time to carry out the operations for each sample
            The average and standard deviation of the times for the given number of samples
 
-----------------------------------------------------------------------------------------------------------------------------------

File:     mutex_linked_list.c
 
Purpose:  Implement a serial sorted linked list of
            ints with operations insert, member, delete, free list.
            This version uses mutex locks
 
Compile:  gcc -g -Wall -std=gnu99 mutex_linked_list.c -o mutex_linked_list -lpthread -lm
Usage:    ./mutex_linked_list 
Input:    total number of samples
            total number of keys inserted by main thread
            total number of operations
            percent of operations that are search, insert (remainder are delete)
Output:   The average and standard deviation of the times for the given number of samples
 
------------------------------------------------------------------------------------------------------------------------------------

  File:     pthread_linked_list_readwritelock.c
 
  Purpose:  Implement a multi-threaded sorted linked list of
            ints with operations insert member, delete, free list.
            This version uses read-write locks
 
  Compile:  gcc -g -Wall -std=gnu99 pthread_linked_list_readwritelock.c -o pthread_linked_list_readwritelock -lpthread -lm
  Usage:    ./pthread_linked_list_readwritelock
  Input:    total number of samples
            total number of keys inserted by main thread
            total number of operations
            percent of operations that are search, insert (remainder are delete)
  Output:   Elapsed time to carry out the operations for each sample
            The average and standard deviation of the times for the given number of samples
 
