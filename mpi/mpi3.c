#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

int *create_nn_matrix(int n) {
    int *matrix = (int *)malloc(sizeof(int) * n * n);
    for (int i = 0; i < n * n; i++) {
        matrix[i] = i;
    }
    return matrix;
}

void print_array(int *array, int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
}

void create_upper_triangular_matrix(int *matrix, int n, MPI_Datatype *input_mpi_t_p) {
    int *array_of_blocklengths = (int *)malloc(sizeof(int) * n);
    int *array_of_displacements = (int *)malloc(sizeof(int) * n);
    //MPI_Aint start_addr = 0, end_addr = 0;
    for (int i = 0; i < n; i++) {
        array_of_blocklengths[i] = n - i;
        array_of_displacements[i] = i * 4 + i;
        //array_of_displacements[i] = end_addr - start_addr;
        //MPI_Get_address(&(matrix[i * n + i]), &start_addr);
        //MPI_Get_address(&(matrix[(i + 1) * n]), &end_addr);
    }
    print_array(array_of_blocklengths, n);
    print_array(array_of_displacements, n);
    MPI_Type_indexed(n, array_of_blocklengths, array_of_displacements, MPI_INT, input_mpi_t_p);
    free(array_of_blocklengths);
    free(array_of_displacements);
    MPI_Type_commit(input_mpi_t_p);
}

int main(int argc, char *argv[]) {
    if (argc != 2) exit(1);
    int n = atoi(argv[1]);

    int local_rank, comm_sz;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &local_rank);

    if (local_rank == 0) {
        int *matrix = create_nn_matrix(n);
        print_array(matrix, n * n);
        MPI_Datatype ut_matrix;
        create_upper_triangular_matrix(matrix, n, &ut_matrix);
        MPI_Send(matrix, 1, ut_matrix, 1, 0, MPI_COMM_WORLD);
        MPI_Type_free(&ut_matrix);
        free(matrix);
    } else {
        int ut_matrix_size = (n + 1) * n / 2;
        int *data = (int *)malloc(sizeof(int) * ut_matrix_size);
        MPI_Status recv_status;
        int recv_count;
        MPI_Recv(data, ut_matrix_size, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &recv_status);
        MPI_Get_count(&recv_status, MPI_INT, &recv_count);
        if (recv_count == ut_matrix_size) {
            print_array(data, ut_matrix_size);
        } else {
            printf("%d %d\n", ut_matrix_size, recv_count);
        }
        free(data);
    }

    MPI_Finalize();
    return 0;
}