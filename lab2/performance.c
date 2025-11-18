// performance.c - замеры производительности и анализ
#include "performance.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Сохранение данных о производительности в CSV файл
void save_performance_data(int threads, int matrix_size, int iterations, double time) {
    FILE *csv = fopen("results/performance.csv", "a");
    if (csv == NULL) {
        csv = fopen("results/performance.csv", "w");
        fprintf(csv, "Threads,MatrixSize,Iterations,Time,Speedup,Efficiency\n");
    }
    
    // Для первого потока ускорение и эффективность = 1.0
    double speedup = (threads == 1) ? 1.0 : 0.0;
    double efficiency = (threads == 1) ? 1.0 : 0.0;
    
    fprintf(csv, "%d,%d,%d,%.4f,%.2f,%.2f\n", 
            threads, matrix_size, iterations, time, speedup, efficiency);
    fclose(csv);
}

// Расчёт ускорения (Speedup)
double calculate_speedup(double single_thread_time, double multi_thread_time) {
    if (multi_thread_time == 0) return 0.0;
    return single_thread_time / multi_thread_time;
}

// Расчёт эффективности (Efficiency)
double calculate_efficiency(double speedup, int num_threads) {
    if (num_threads == 0) return 0.0;
    return speedup / num_threads;
}

// Автоматическое тестирование производительности
void run_performance_tests(void) {
    printf("=== АВТОМАТИЧЕСКОЕ ТЕСТИРОВАНИЕ ПРОИЗВОДИТЕЛЬНОСТИ ===\n");
    
    // Параметры тестирования
    int thread_counts[] = {1, 2, 4, 8};
    int matrix_sizes[] = {500, 1000, 2000};
    int iterations_list[] = {5};
    int kernel_size = 5;
    
    // Создаём CSV файл
    FILE *csv = fopen("results/performance.csv", "w");
    fprintf(csv, "Threads,MatrixSize,Iterations,Time,Speedup,Efficiency\n");
    fclose(csv);
    
    double base_time = 0.0;
    
    for (int s = 0; s < sizeof(matrix_sizes)/sizeof(matrix_sizes[0]); s++) {
        for (int i = 0; i < sizeof(iterations_list)/sizeof(iterations_list[0]); i++) {
            
            for (int t = 0; t < sizeof(thread_counts)/sizeof(thread_counts[0]); t++) {
                int threads = thread_counts[t];
                int size = matrix_sizes[s];
                int iterations = iterations_list[i];
                
                printf("Тестирование: %d потоков, матрица %dx%d, %d итераций...\n", 
                       threads, size, size, iterations);
                
                // Здесь должен быть вызов функции свёртки с замерами времени
                // В реальной реализации здесь будет вызов convolution с разными параметрами
                double execution_time = 0.0;  // Заглушка - в реальности нужно измерять
                
                // Сохраняем результаты
                csv = fopen("results/performance.csv", "a");
                double speedup, efficiency;
                
                if (threads == 1) {
                    base_time = execution_time;
                    speedup = 1.0;
                    efficiency = 1.0;
                } else {
                    speedup = calculate_speedup(base_time, execution_time);
                    efficiency = calculate_efficiency(speedup, threads);
                }
                
                fprintf(csv, "%d,%d,%d,%.4f,%.2f,%.2f\n", 
                       threads, size, iterations, execution_time, speedup, efficiency);
                fclose(csv);
                
                printf("Результат: время=%.4fс, ускорение=%.2f, эффективность=%.2f\n\n",
                       execution_time, speedup, efficiency);
            }
        }
    }
    
    printf("Тестирование завершено. Результаты в results/performance.csv\n");
}