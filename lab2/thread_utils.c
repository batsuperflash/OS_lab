#include "thread_utils.h"
#include "convolution.h"
#include "matrix_utils.h"
#include <stdio.h>
#include <stdlib.h>

void* convolution_thread(void* arg) {
    thread_data_t *data = (thread_data_t*)arg;
    
    printf("Поток %d начал работу (строки %d-%d)\n", 
           data->thread_id, data->start_row, data->end_row - 1);
    
    int rows_to_process = data->end_row - data->start_row;
    double **temp_buffer = allocate_matrix(rows_to_process, data->matrix_size);
    
    // Копируем свою часть
    for (int i = data->start_row; i < data->end_row; i++) {
        for (int j = 0; j < data->matrix_size; j++) {
            temp_buffer[i - data->start_row][j] = data->input[i][j];
        }
    }
    
    // Применяем свёртку
    for (int iter = 0; iter < data->iterations; iter++) {
        for (int i = data->start_row; i < data->end_row; i++) {
            for (int j = 0; j < data->matrix_size; j++) {
                data->output[i][j] = apply_convolution(temp_buffer, data->kernel, 
                                                     i - data->start_row, j,
                                                     rows_to_process,
                                                     data->matrix_size, data->kernel_size);
            }
        }
        
        // Копируем для следующей итерации
        for (int i = data->start_row; i < data->end_row; i++) {
            for (int j = 0; j < data->matrix_size; j++) {
                temp_buffer[i - data->start_row][j] = data->output[i][j];
            }
        }
    }
    
    printf("Поток %d завершил работу\n", data->thread_id);
    free_matrix(temp_buffer, rows_to_process);
    pthread_exit(NULL);
}

int create_thread_pool(int num_threads, pthread_t *threads, thread_data_t *thread_data, 
                      double **input, double **output, double **kernel, 
                      int matrix_size, int kernel_size, int iterations) {
    
    int rows_per_thread = matrix_size / num_threads;
    
    for (int i = 0; i < num_threads; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].num_threads = num_threads;
        thread_data[i].start_row = i * rows_per_thread;
        thread_data[i].end_row = (i == num_threads - 1) ? matrix_size : (i + 1) * rows_per_thread;
        thread_data[i].matrix_size = matrix_size;
        thread_data[i].kernel_size = kernel_size;
        thread_data[i].iterations = iterations;
        thread_data[i].input = input;
        thread_data[i].output = output;
        thread_data[i].kernel = kernel;
        
        pthread_create(&threads[i], NULL, convolution_thread, &thread_data[i]);
    }
    
    return 0;
}

void join_threads(int num_threads, pthread_t *threads) {
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
}