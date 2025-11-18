#ifndef MATRIX_UTILS_H
#define MATRIX_UTILS_H

double** create_random_matrix(int size);
double** allocate_matrix(int rows, int cols);
void free_matrix(double **matrix, int rows);

#endif