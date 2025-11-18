#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "thread_utils.h"
#include "matrix_utils.h"
#include "convolution.h"

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Использование: %s <потоки> <размер_матрицы> <размер_ядра> <итерации>\n", argv[0]);
        printf("Пример: %s 4 1000 5 10\n", argv[0]);
        return 1;
    }
    
    int num_threads = atoi(argv[1]);
    int matrix_size = atoi(argv[2]);
    int kernel_size = atoi(argv[3]);
    int iterations = atoi(argv[4]);
    
    if (kernel_size % 2 == 0) {
        printf("Ошибка: размер ядра должен быть нечётным! Получено: %d\n", kernel_size);
        return 1;
    }
    
    printf("\n=== МНОГОПОТОЧНАЯ СВЁРТКА ===\n");
    printf("Потоков: %d, Матрица: %dx%d, Ядро: %dx%d, Итераций: %d\n", 
           num_threads, matrix_size, matrix_size, kernel_size, kernel_size, iterations);
    
    // ДЕМОНСТРАЦИЯ МЬЮТЕКСА (только создание/уничтожение)
    pthread_mutex_t demo_mutex;
    pthread_mutex_init(&demo_mutex, NULL);
    printf(" Мьютекс инициализирован для демонстрации\n");
    
    // Выделение памяти
    double **input_matrix = create_random_matrix(matrix_size);
    double **output_matrix = allocate_matrix(matrix_size, matrix_size);
    double **kernel = create_blur_kernel(kernel_size);
    
    // ЗАМЕР ВРЕМЕНИ НАЧАЛА
    clock_t start_time = clock();
    
    // Создание потоков
    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
    thread_data_t *thread_data = malloc(num_threads * sizeof(thread_data_t));
    
    create_thread_pool(num_threads, threads, thread_data, input_matrix, output_matrix, 
                      kernel, matrix_size, kernel_size, iterations);
    
    // Ожидание завершения потоков
    join_threads(num_threads, threads);
    
    // ЗАМЕР ВРЕМЕНИ ОКОНЧАНИЯ
    clock_t end_time = clock();
    double execution_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    
    // Вывод результатов
    printf("\n=== РЕЗУЛЬТАТЫ ===\n");
    printf("Время выполнения: %.4f секунд\n", execution_time);
    printf("Потоков: %d, Размер: %dx%d, Итераций: %d\n", 
           num_threads, matrix_size, matrix_size, iterations);
    
    // ДЕМОНСТРАЦИЯ МЬЮТЕКСА (корректное уничтожение)
    pthread_mutex_destroy(&demo_mutex);
    printf(" Мьютекс уничтожен\n");
    
    // Освобождение памяти
    free_matrix(input_matrix, matrix_size);
    free_matrix(output_matrix, matrix_size);
    free_matrix(kernel, kernel_size);
    free(threads);
    free(thread_data);
    
    printf("Программа завершена успешно!\n");
    return 0;
}