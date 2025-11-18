#include "convolution.h"
#include "matrix_utils.h"
#include <stdlib.h>

double apply_convolution(double **matrix, double **kernel, int i, int j,
                        int matrix_rows, int matrix_cols, int kernel_size) {
    double sum = 0.0;
    int k_half = kernel_size / 2;
    
    for (int ki = -k_half; ki <= k_half; ki++) {
        for (int kj = -k_half; kj <= k_half; kj++) {
            int ni = i + ki;
            int nj = j + kj;
            
            // Обработка границ
            if (ni < 0) ni = -ni;
            if (nj < 0) nj = -nj;
            if (ni >= matrix_rows) ni = 2 * matrix_rows - ni - 2;
            if (nj >= matrix_cols) nj = 2 * matrix_cols - nj - 2;
            
            sum += matrix[ni][nj] * kernel[ki + k_half][kj + k_half];
        }
    }
    
    return sum;
}

double** create_blur_kernel(int size) {
    double **kernel = allocate_matrix(size, size);
    double value = 1.0 / (size * size);
    
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            kernel[i][j] = value;
        }
    }
    return kernel;
}