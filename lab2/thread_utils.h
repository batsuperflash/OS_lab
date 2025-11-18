#ifndef THREAD_UTILS_H
#define THREAD_UTILS_H

#include <pthread.h>

typedef struct {
    int thread_id;
    int num_threads;
    int start_row;
    int end_row;
    int matrix_size;
    int kernel_size;
    int iterations;
    double **input;
    double **output;
    double **kernel;
} thread_data_t;

int create_thread_pool(int num_threads, pthread_t *threads, thread_data_t *thread_data, 
                      double **input, double **output, double **kernel, 
                      int matrix_size, int kernel_size, int iterations);
void* convolution_thread(void* arg);
void join_threads(int num_threads, pthread_t *threads);

#endif