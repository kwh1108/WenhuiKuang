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
volatile int *spin_lock;
pthread_mutex_t *mut;

int num_list = 1;
SortedList_t *list;
int length = 0;
SortedListElement_t *element;

void* thread_func(void *arg)
{
    //SortedListElement_t *element = (SortedListElement_t *)arg;
    int *t = (int*)arg;
    int i;
    for (i = 0; i < num_iterations; i++)
    {
        unsigned int temp1 = element[i + *t].key[0] + element[i + *t].key[1] + element[i + *t].key[2];
        //printf("%d,%d,%d\n",element[i + *t].key[0],element[i + *t].key[1],element[i + *t].key[2]);
        int position = temp1%num_list;
        //printf("%d\n",temp1);
        switch (which_lock) {
            case 'm':
                pthread_mutex_lock(&mut[position]);
                SortedList_insert(&list[position],&element[i + *t]);
                //printf("insert1\n");
                pthread_mutex_unlock(&mut[position]);
                break;
            case 's':
                while (__sync_lock_test_and_set(&spin_lock[position], 1));
                SortedList_insert(&list[position],&element[i + *t]);
                __sync_lock_release(&spin_lock[position]);
                break;
            default:
                SortedList_insert(&list[position],&element[i + *t]);
                break;
        }
    }
//    for (i = 0; i < num_list; i++)
//    {
//        switch (which_lock) {
//            case 'm':
//                pthread_mutex_lock(&mut[i]);
//                break;
//            case 's':
//                while (__sync_lock_test_and_set(&spin_lock[i], 1));
//                break;
//            default:
//                break;
//        }
//        
//    }
//    for (i = 0; i < num_list; i++)
//    {
//        length += SortedList_length(&list[i]);
//    }
//    for (i = 0; i < num_list; i++)
//    {
//        switch (which_lock) {
//            case 'm':
//                pthread_mutex_unlock(&mut[i]);
//                break;
//            case 's':
//                __sync_lock_release(&spin_lock[i]);
//                break;
//            default:
//                break;
//        }
//    }
    
    for (i = 0; i < num_list; i++)
    {
        switch (which_lock) {
            case 'm':
                pthread_mutex_lock(&mut[i]);
                length = SortedList_length(&list[i]);
                pthread_mutex_unlock(&mut[i]);
                break;
            case 's':
                while (__sync_lock_test_and_set(&spin_lock[i], 1));
                length = SortedList_length(&list[i]);
                __sync_lock_release(&spin_lock[i]);
                break;
            default:
                length = SortedList_length(&list[i]);
                break;
        }

    }

    for (i = 0; i < num_iterations; i++)
    {
        int temp1 = element[i + *t].key[0] + element[i + *t].key[1] + element[i + *t].key[2];
        int position = temp1%num_list;
        SortedListElement_t *temp;
        switch (which_lock) {
            case 'm':
                pthread_mutex_lock(&mut[position]);
                temp = SortedList_lookup(&list[position], element[i + *t].key);
                SortedList_delete(temp);
                pthread_mutex_unlock(&mut[position]);
                break;
            case 's':
                while (__sync_lock_test_and_set(&spin_lock[position], 1));
                temp = SortedList_lookup(&list[position], element[i + *t].key);
                SortedList_delete(temp);
                __sync_lock_release(&spin_lock[position]);
                break;
            default:
                temp = SortedList_lookup(&list[position], element[i + *t].key);
                SortedList_delete(temp);
                break;
        }
    }
    //fprintf(stdout, "%d\n",length);
    return NULL;
}

void freeFunction()
{
    int i;
    for (i = 0; i < num_iterations*num_threads; i++) {
        free((void*)element[i].key);
    }
    free(element);
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
        {"lists", required_argument, NULL, 'l'},
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
                break;
            case 'l':
                num_list = atoi(optarg);
                break;
                
        }
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
    //Create empty lists.
    list = (SortedList_t*)malloc(num_list*sizeof(SortedList_t));
    spin_lock = (int*)malloc(num_list*sizeof(int));
    mut = (pthread_mutex_t*)malloc(num_list*sizeof(pthread_mutex_t));
    int i;
    for (i = 0; i < num_list; i++) {
        list[i].next = &list[i];
        list[i].prev = &list[i];
        list[i].key = NULL;
        spin_lock[i] = 0;
        pthread_mutex_init(&mut[i], NULL);
    }
    
    num_operations = num_iterations*num_threads;
    srand(time(NULL));
    int numOfKeys;
    element = (SortedListElement_t *)malloc(sizeof(SortedListElement_t) * num_operations);
    for (numOfKeys = 0; numOfKeys < num_operations; numOfKeys++) {
        char *keys = (char*)malloc(5*sizeof(char));
        for (i = 0; i < 4; i++) {
            char number = (char)(rand() % 67 + 30);
            keys[i] = number;
        }
        keys[4] = '\0';
        
        element[numOfKeys].key = keys;
    }
    
    atexit(freeFunction);
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
    
    
    num_operations = num_threads * num_iterations;
    num_operations *= 2;
    printf("%d threads x %d iterations x (insert + lookup/delete) = %d\n",num_threads, num_iterations, num_operations);
//    if (length != 0) {
//        printf("elapsed time: %ldns\n", elapsed);
//        printf("per operation: %ldns\n",elapsed/num_operations);
//        exit(1);
//    }
//    else
//    {
    printf("elapsed time: %ldns\n", elapsed);
    printf("per operation: %ldns\n",elapsed/num_operations);
    printf("Corrected per operation: %fns\n",((double)elapsed/num_operations)/(double)(num_operations/num_list/2));
    exit(0);
//


}
