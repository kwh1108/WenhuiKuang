//
//  main.c
//  lab2a
//
//  Created by wenhui kuang on 4/20/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <getopt.h>
#include <time.h>

#define required_argument 1
#define no_qrgument 0
#define optional_argument 2

long long counter = 0;
int num_iterations;
int opt_yield = 0;
char which_lock;
volatile int lock = 0;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

void add(long long *pointer, long long value) {
    long long sum = *pointer + value;
    if (opt_yield)
        pthread_yield();
    *pointer = sum;
}

void add1(long long *pointer, long long value) {
    long long prev,sum;
    do
    {
        prev = *pointer;
        sum = prev + value;
        if (opt_yield)
            pthread_yield();
    } while (__sync_val_compare_and_swap(pointer,prev,sum) != prev);
}

void* thread_func(void *arg)
{
    int i;
    for (i = 0; i < num_iterations; i++)
    {
        switch (which_lock) {
            case 'm':
                pthread_mutex_lock(&mut);
                add((long long *)arg, 1);
                pthread_mutex_unlock(&mut);
                break;
            case 's':
                while (__sync_lock_test_and_set(&lock, 1));
                add((long long *)arg, 1);
                __sync_lock_release(&lock);
                break;
            case 'c':
                add1((long long *)arg, 1);
                break;
            default:
                add((long long *)arg, 1);
                break;
        }
    }
    for (i = 0; i < num_iterations; i++)
    {
        switch (which_lock) {
            case 'm':
                pthread_mutex_lock(&mut);
                add((long long *)arg, -1);
                pthread_mutex_unlock(&mut);
                break;
            case 's':
                while (__sync_lock_test_and_set(&lock, 1));
                add((long long *)arg, -1);
                __sync_lock_release(&lock);
                break;
            case 'c':
                add1((long long *)arg, -1);
                break;
            default:
                add((long long *)arg, -1);
                break;
        }
    }
    return NULL;
}


int main(int argc, char ** argv)
{
    int num_threads, num_operations = 0;
    char *output1 = NULL, *output2 = NULL;
    struct timespec start, finish;
    long elapsed;
    
    const struct option longopts[] =
    {
        {"iterations", optional_argument, NULL, 'i'},
        {"threads", optional_argument, NULL, 't'},
        {"yield", no_argument, NULL, 'y'},
        {"sync", required_argument, NULL, 's'},
    };
    int iarg;
    while ((iarg = getopt_long(argc, argv,"i:t:ys:",longopts,NULL)) != -1){
        switch (iarg){
            case 'i':;
                output1 = optarg;
                break;
            case 't':
                output2 = optarg;
                break;
            case 'y':
                opt_yield = 1;
                break;
            case 's':
                which_lock = *optarg;
                
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
    clock_gettime(CLOCK_MONOTONIC, &start);
    pthread_t tid[num_threads];
    int t;
    void *status;
    for (t = 0; t < num_threads; t++)
    {
        int ret = pthread_create(&tid[t], NULL, thread_func, &counter);
        if (ret) {
            printf("Error creating thread. Error code is %d\n", ret);
            exit(1);
        }
    }
    for(t = 0; t < num_threads; t++) {
        int ret = pthread_join(tid[t], &status);
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
    printf("%d threads x %d iterations x (add + subtract) = %d\n",num_threads, num_iterations, num_operations);
    if (counter != 0) {
        fprintf(stderr, "ERROR : final count = %lld\n", counter);
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
    return 0;
}
