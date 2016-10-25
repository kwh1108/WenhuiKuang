//
//  main.c
//  lab2b
//
//  Created by wenhui kuang on 4/29/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include <pthread.h>

#include "SortedList.h"

#define required_argument 1

int num_iterations, num_threads;
int opt_yield = 0;
char which_lock;
char *which_conflict = NULL;
volatile int lock = 0;
pthread_mutex_t mut;

SortedList_t list;
int length;
SortedListElement_t *element;

void* thread_func(void *arg)
{
    //SortedListElement_t *element = (SortedListElement_t *)arg;
    int *t = (int*)arg;
    int i;
    for (i = 0; i < num_iterations; i++)
    {
        switch (which_lock) {
            case 'm':
                pthread_mutex_lock(&mut);
                SortedList_insert(&list,&element[i + *t]);
                //printf("insert1\n");
                pthread_mutex_unlock(&mut);
                break;
            case 's':
                while (__sync_lock_test_and_set(&lock, 1));
                SortedList_insert(&list,&element[i + *t]);
                __sync_lock_release(&lock);
                break;
            default:
                SortedList_insert(&list,&element[i + *t]);
                break;
        }
    }
    switch (which_lock) {
        case 'm':
            pthread_mutex_lock(&mut);
            length = SortedList_length(&list);
            pthread_mutex_unlock(&mut);
            break;
        case 's':
            while (__sync_lock_test_and_set(&lock, 1));
            length = SortedList_length(&list);
            __sync_lock_release(&lock);
            break;
        default:
            length = SortedList_length(&list);
            break;
    }

    for (i = 0; i < num_iterations; i++)
    {
        SortedListElement_t *temp;
        switch (which_lock) {
            case 'm':
                pthread_mutex_lock(&mut);
                temp = SortedList_lookup(&list, element[i + *t].key);
                SortedList_delete(temp);
                pthread_mutex_unlock(&mut);
                break;
            case 's':
                while (__sync_lock_test_and_set(&lock, 1));
                temp = SortedList_lookup(&list, element[i + *t].key);
                SortedList_delete(temp);
                __sync_lock_release(&lock);
                break;
            default:
                temp = SortedList_lookup(&list, element[i + *t].key);
                SortedList_delete(temp);
                break;
        }
    }
    //fprintf(stdout, "%d\n",length);
    return NULL;
}

int main(int argc, char ** argv)
{
    int num_operations = 0;
    char *output1 = NULL, *output2 = NULL;
    struct timespec start, finish;
    long elapsed;
    //SortedListElement_t *element;
    
    const struct option longopts[] =
    {
        {"iterations", required_argument, NULL, 'i'},
        {"threads", required_argument, NULL, 't'},
        {"yield", required_argument, NULL, 'y'},
        {"sync", required_argument, NULL, 's'},
    };
    int iarg;
    while ((iarg = getopt_long(argc, argv,"i:t:y:s:",longopts,NULL)) != -1){
        switch (iarg){
            case 'i':;
                output1 = optarg;
                break;
            case 't':
                output2 = optarg;
                break;
            case 'y':
                which_conflict = optarg;
                break;
            case 's':
                which_lock = *optarg;
                
        }
    }
    if (which_lock == 'm') {
        pthread_mutex_init(&mut, NULL);
    }
    if (which_conflict != NULL) {
        if (strcmp(which_conflict, "i") == 0) {
            opt_yield |= INSERT_YIELD;
        }
        else if (strcmp(which_conflict, "d") == 0)
        {
            opt_yield |= DELETE_YIELD;
        }
        else if (strcmp(which_conflict, "is") == 0)
        {
            opt_yield |= INSERT_YIELD;
            opt_yield |= SEARCH_YIELD;
        }
        else
        {
            opt_yield |= DELETE_YIELD;
            opt_yield |= SEARCH_YIELD;
        }

    }

    if (output1 == NULL && output2 == NULL)
    {
        num_iterations = 1;
        num_threads = 1;
    }
    else if (output1 == NULL)
    {
        num_threads = atoi(output2);
        num_iterations = 1;
    }
    else if(output2 == NULL)
    {
        num_iterations = atoi(output1);
        num_threads = 1;
    }
    else
    {
        num_threads = atoi(output2);
        num_iterations = atoi(output1);
    }
    //Create empty list.
    list.next = &list;
    list.key = NULL;
    list.prev = &list;
    
    num_operations = num_iterations*num_threads;
    //srand(time(NULL));
    int numOfKeys;
    int i;
    element = (SortedListElement_t *)malloc(sizeof(SortedListElement_t) * num_operations);
    for (numOfKeys = 0; numOfKeys < num_operations; numOfKeys++) {
        char *keys = (char*)malloc(5*sizeof(char));
        for (i = 0; i < 4; i++) {
            char number = (char)(rand() % 67 + 65);
            keys[i] = number;
        }
        keys[4] = '\0';
        
        element[numOfKeys].key = keys;
    }
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    int *offsets = (int *)malloc(sizeof(int) * num_threads);
    for (i = 0; i < num_threads; i++) {
        offsets[i] = i * num_iterations;
    }
    
    pthread_t tid[num_threads];
    //void *status;
    int t;
    //int num_t[num_threads];
    for (t = 0; t < num_threads; t++)
    {
        int ret = pthread_create(&(tid[t]), NULL, thread_func, &offsets[t]);
        if (ret) {
            printf("Error creating thread. Error code is %d\n", ret);
            exit(1);
        }
    }
    for(t = 0; t < num_threads; t++) {
        int ret = pthread_join(tid[t], NULL);
        if (ret)
        {
            printf("ERROR; return code from pthread_join()is %d\n", ret);
            exit(1);
        }
        //printf("Main: completed join with thread %ld having a statusof %ld\n",t,(long)status);
    }
    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed = (finish.tv_sec - start.tv_sec) * 1000000000;
    elapsed += (finish.tv_nsec - start.tv_nsec);
    
    
    length = SortedList_length(&list);
    num_operations = num_threads * num_iterations;
    num_operations *= 2;
    printf("%d threads x %d iterations x (insert + lookup/delete) = %d\n",num_threads, num_iterations, num_operations);
    if (length != 0) {
        fprintf(stderr, "ERROR : corrupted list.\n");
        printf("elapsed time: %ldns\n", elapsed);
        printf("per operation: %ldns\n",elapsed/num_operations);
        exit(1);
    }
    else
    {
        printf("elapsed time: %ldns\n", elapsed);
        printf("per operation: %ldns\n",elapsed/num_operations);
        exit(0);
    }


}
