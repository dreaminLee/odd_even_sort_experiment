#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include "util.h"

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int comm_size, local_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &local_rank);
    int *rand_array = NULL, n = 0, read_ok = 0;
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
        printf("ok %d\n", local_rank);
        
        MPI_Finalize();
        return 0;
    }
}