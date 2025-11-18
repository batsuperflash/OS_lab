#include "matrix_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double** create_random_matrix(int size) {
    double **matrix = allocate_matrix(size, size);
    srand(time(NULL));
    
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            matrix[i][j] = (double)rand() / RAND_MAX * 100.0;
        }
    }
    return matrix;
}

double** allocate_matrix(int rows, int cols) {
    double **matrix = (double**)malloc(rows * sizeof(double*));
    for (int i = 0; i < rows; i++) {
        matrix[i] = (double*)malloc(cols * sizeof(double));
    }
    return matrix;
}

void free_matrix(double **matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}