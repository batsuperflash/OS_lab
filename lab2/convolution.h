#ifndef CONVOLUTION_H
#define CONVOLUTION_H

double apply_convolution(double **matrix, double **kernel, int i, int j,
                        int matrix_rows, int matrix_cols, int kernel_size);
double** create_blur_kernel(int size);

#endif