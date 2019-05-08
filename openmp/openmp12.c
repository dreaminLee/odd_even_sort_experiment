#include <stdlib.h>
#include <omp.h>
#include <string.h>

void solve_by_row(int **upper_triangular_matrix, int *b_matrix, int n, int **solution) {
    int *x = malloc(sizeof(int) * n);
    for (int row = n-1; row >= 0; row--) {
        x[row] = b_matrix[row];
        for (int col = row+1; col < n; col++) {
            x[row] -= upper_triangular_matrix[row][col] * x[col];
        }
        x[row] /= upper_triangular_matrix[row][row];
    }
    *solution = x;
}

void solve_by_column(int **upper_triangular_matrix, int *b_matrix, int n, int **solution) {
    int *x = malloc(sizeof(int) * n);
    for (int row = 0; row < n; row++) {
        x[row] = b_matrix[row];
    }
    for (int col = n-1; col >= 0; col--) {
        x[col] /= upper_triangular_matrix[col][col];
        for (int row = 0; row < n; row++) {
            x[row] -= upper_triangular_matrix[row][col] * x[col];
        }
    }
    *solution = x;
}

/*
    a. outer loop can't be parallel
    b. inner loop can be parallel but may cost more resource
*/
void solve_by_row_openmp(int **upper_triangular_matrix, int *b_matrix, int n, int **solution) {
    int *x = malloc(sizeof(int) * n);
    for (int row = n-1; row >= 0; row--) {
        x[row] = b_matrix[row];
        for (int col = row+1; col < n; col++) {
            x[row] -= upper_triangular_matrix[row][col] * x[col];
        }
        x[row] /= upper_triangular_matrix[row][row];
    }
    *solution = x;
}

/*
    c. outer loop can't be parallel
    d. inner loop can be parallel
*/
void solve_by_column_openmp(int **upper_triangular_matrix, int *b_matrix, int n, int **solution) {
    int *x = malloc(sizeof(int) * n);
    memcpy(x, b_matrix, sizeof(int) * n);
    for (int col = n-1; col >= 0; col--) {
        x[col] /= upper_triangular_matrix[col][col];
#       pragma omp parallel for default(none) private(row) shared(n, x, upper_triangular_matrix, col)
        for (int row = 0; row < n; row++) {
            x[row] -= upper_triangular_matrix[row][col] * x[col];
        }
    }
    *solution = x;
}

int main(int argc, char *argv[]) {
    return 0;
}