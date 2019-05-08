#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

int *create_nn_matrix(int n) {
    int *matrix = (int *)malloc(sizeof(int) * n * n);
    for (int i = 0; i < n * n; i++) {
        matrix[i] = rand() % 10;
    }
    return matrix;
}

int *create_n_vector(int n) {
    int *vector = (int *)malloc(sizeof(int) * n * n);
    for (int i = 0; i < n; i++) {
        vector[i] = rand() % 10;
    }
    return vector;
}

void print_column(int *array, int column, int elem_per_col) {
    for (int i = 0; i < elem_per_col; i++) {
        for (int j = 0; j < column; j++) {
            printf("%d ", array[i + j * elem_per_col]);
        }
        printf("\n");
    }
}

void print_matrix(int *matrix, int row, int column) {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            printf("%d ", matrix[i * column + j]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) exit(1);
    int n = atoi(argv[1]);
    srand(0);

    int local_rank, comm_sz;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &local_rank);

    /*
         0 0 0 0 1 1 1 1
        |x|x|x|x|x|x|x|x|
        |.|.|.|.|.|.|.|.|
        |.|.|.|.|.|.|.|.|
        |.|.|.|.|.|.|.|.|
        |.|.|.|.|.|.|.|.|
    */

    int column_per_proc = n / comm_sz;
    int *matrix = NULL;
    int *local_matrix_column = (int *)malloc(sizeof(int) * column_per_proc * n);
    int *vector = NULL;
    if (local_rank == 0) {
        matrix = create_nn_matrix(n);
        free(vector);
        vector = create_n_vector(n);
        print_matrix(matrix, n, n);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < column_per_proc; j++) {
                local_matrix_column[i + j * n] = matrix[i * n + j];
            }
        }
        //print_column(local_matrix_column, 2, n);
        for (int proc = 1; proc < comm_sz; proc++) {
            for (int row = 0; row < n; row++) {
                MPI_Send(&(matrix[row * n + proc * column_per_proc]), column_per_proc, MPI_INT, proc, row, MPI_COMM_WORLD);
            }
        }
    } else {
        int *temp = (int *)malloc(sizeof(int) * column_per_proc);
        for (int row = 0; row < n; row++) {
            MPI_Recv(temp, column_per_proc, MPI_INT, 0, row, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (int j = 0; j < column_per_proc; j++) {
                local_matrix_column[j * n + row] = temp[j];
            }
        }
        free(temp);
        //print_column(local_matrix_column, column_per_proc, n);
    }
    int *part_vector = (int *)malloc(sizeof(int) * column_per_proc);
    MPI_Scatter(vector, column_per_proc, MPI_INT, 
                part_vector, column_per_proc, MPI_INT, 
                0, MPI_COMM_WORLD);

    int *local_result = (int *)malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++) {
        local_result[i] = 0;
        for (int j = 0; j < column_per_proc; j++) {
            local_result[i] += part_vector[j] * local_matrix_column[i + n * j];
        }
    }

    int *result = NULL;
    if (local_rank == 0) {
        result = (int *)malloc(sizeof(int) * n);
    }
    
    MPI_Reduce(local_result, result, n, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (local_rank == 0) {
        print_column(vector, 1, n);
        print_column(result, 1, n);
    }

    MPI_Finalize();
    return 0;
}
