#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

char ping[] = "ping";
char pong[] = "pong";

void elapse_MPI_Wtime(int times) {
    int local_rank;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &local_rank);

    if (local_rank == 0) {
        char temp[5] = {0};
        double start, finish, elapsed;
        start = MPI_Wtime();
        for (int i = 0; i < times; i++) {
            MPI_Send(ping, 5, MPI_CHAR, 1, i, MPI_COMM_WORLD);
            MPI_Recv(temp, 5, MPI_CHAR, 1, i+1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        finish = MPI_Wtime();
        elapsed = finish - start;
        printf("%e seconds\n", elapsed / 100);
    } else if (local_rank == 1) {
        char temp[5] = {0};
        for (int i = 0; i < times; i++) {
            MPI_Recv(temp, 5, MPI_CHAR, 0, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(pong, 5, MPI_CHAR, 0, i+1, MPI_COMM_WORLD);
        }
    }
}

void elapse_clock(int times) {
    int local_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &local_rank);

    if (local_rank == 0) {
        char temp[5] = {0};
        clock_t start, finish, elapsed;
        start = clock();
        for (int i = 0; i < times; i++) {
            MPI_Send(ping, 5, MPI_CHAR, 1, i, MPI_COMM_WORLD);
            MPI_Recv(temp, 5, MPI_CHAR, 1, i+1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        finish = clock();
        elapsed = finish - start;
        printf("%e seconds\n", (double)elapsed / CLOCKS_PER_SEC / 100);
    } else if (local_rank == 1) {
        char temp[5] = {0};
        for (int i = 0; i < times; i++) {
            MPI_Recv(temp, 5, MPI_CHAR, 0, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(pong, 5, MPI_CHAR, 0, i+1, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
}

int main() {
    elapse_MPI_Wtime(1000000);
    elapse_clock(1000000);
    return 0;
}