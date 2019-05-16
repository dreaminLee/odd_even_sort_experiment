#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../util/util.h"
//#define DEBUG 1

/*
    array: array to be sorted, NULL in processes other than process 0
    n: size of array
    rank: rank of process
    comm_size: size of communicators, must divide n exactly
*/
void parallel_sort(int *array, int n, int rank, int comm_size) {
    int *local_sub_array = NULL, local_sub_n = n / comm_size;
    local_sub_array = malloc(sizeof(int) * local_sub_n);
    MPI_Scatter(array, local_sub_n, MPI_INT, 
                local_sub_array, local_sub_n, MPI_INT, 
                0, MPI_COMM_WORLD);
    
    // locally sort part
    qsort(local_sub_array, local_sub_n, sizeof(int), cmp);
    // exchange key and merge part
    int *recv_temp = malloc(sizeof(int) * local_sub_n);
    int *merge_temp = malloc(sizeof(int) * local_sub_n);
    for (int phase = 0; phase < comm_size; phase++) {
        int partner = compute_partner(phase, rank, comm_size);
        if (partner != MPI_PROC_NULL) {
            // Send local keys and receive keys from partner
            #ifdef DEBUG
            printf("%d %d %d\n", phase, rank, partner);
            #endif
            MPI_Sendrecv(local_sub_array, local_sub_n, MPI_INT, partner, phase,
                         recv_temp, local_sub_n, MPI_INT, partner, phase,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            #ifdef DEBUG
            printf("send and receive completed.\n");
            #endif
            if (rank < partner) {
                merge_low(local_sub_array, recv_temp, merge_temp, local_sub_n);
            } else {
                merge_high(local_sub_array, recv_temp, merge_temp, local_sub_n);
            }
        }
    }
    // Gather to original array
    MPI_Gather(local_sub_array, local_sub_n, MPI_INT,
               array, local_sub_n, MPI_INT,
               0, MPI_COMM_WORLD);
    free(recv_temp);
    free(merge_temp);
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int comm_size, local_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &local_rank);
    /* n must be exactly divided by comm_size */
    int *rand_array = NULL, n = 0, read_ok = 0;
    /* read random numbers from file */
    if (local_rank == 0) {
        if (argc != 2) {
            printf("Usage: program <filename>\n");
        } else {
            char *filename = argv[1];
            rand_array = read_rand_array_from_file(filename, &n);
            if (rand_array == NULL) {
                printf("can't create random array from file %s\n", filename);
            } else {
                read_ok = 1;
            }
        }
    }
    MPI_Bcast(&read_ok, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (!read_ok) {
        MPI_Finalize();
        return 0;
    } else {
        double local_start, local_finish, local_elapsed, elapsed;
        MPI_Barrier(MPI_COMM_WORLD);
        local_start = MPI_Wtime();
        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
        parallel_sort(rand_array, n, local_rank, comm_size);
        local_finish = MPI_Wtime();
        local_elapsed = local_finish - local_start;
        MPI_Reduce(&local_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
        if (local_rank == 0) {
            printf("%f\n", elapsed);
            #ifdef DEBUG
            print_array(rand_array, n);
            #endif
        }
        MPI_Finalize();
        return 0;
    }
}