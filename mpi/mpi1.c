#include <mpi.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "timer.h"

//MPI_Sendrecv_replace()
//MPI_Allgather()

float *create_rand_nums(int);
float compute_avg(float *, int);
float procedure_all_gather(float *, int, int, MPI_Comm);
float procedure_sendrecv_replace(float *, int, int, int, MPI_Comm);

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int local_rank, comm_sz;
    MPI_Comm_rank(MPI_COMM_WORLD, &local_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    float *rand_nums = NULL; // the original random array generated from process 0
    int n = 0;
    if (local_rank == 0) {
        printf("numbers per process: ");
        scanf("%d", &n);
        rand_nums = create_rand_nums(comm_sz * n);
    }

    // array scattered to each process
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    float *sub_rand_num = (float *)malloc(sizeof(float) * n);
    MPI_Scatter(rand_nums, 
                n, 
                MPI_FLOAT, 
                sub_rand_num, 
                n, 
                MPI_FLOAT, 0, MPI_COMM_WORLD);

    double local_start, local_finish, local_elapsed, elapsed;
    MPI_Barrier(MPI_COMM_WORLD);
    
    local_start = MPI_Wtime();
    float avg = procedure_all_gather(sub_rand_num, n, comm_sz, MPI_COMM_WORLD);
    local_finish = MPI_Wtime();

    local_elapsed = local_finish - local_start;
    MPI_Reduce(&local_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if (local_rank == 0) {
        printf("Result is %e\n", avg);
        printf("MPI_Allgather Elapsed time = %e seconds\n", elapsed);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    local_start = MPI_Wtime();
    avg = procedure_sendrecv_replace(sub_rand_num, n, local_rank, comm_sz, MPI_COMM_WORLD);
    local_finish = MPI_Wtime();

    local_elapsed = local_finish - local_start;
    MPI_Reduce(&local_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if (local_rank == 0) {
        printf("Result is %e\n", avg);
        printf("MPI_Sendrecv_replace Elapsed time = %e seconds\n", elapsed);
    }

    MPI_Finalize();
    return 0;
}

float procedure_all_gather(float *num, int n, int comm_sz, MPI_Comm comm) {
    float temp = compute_avg(num, n);
    float *sub_avgs = (float *)malloc(sizeof(float) * comm_sz);
    MPI_Allgather(&temp, 1, MPI_FLOAT, sub_avgs, 1, MPI_FLOAT, comm);
    float avg = compute_avg(sub_avgs, comm_sz);
    free(sub_avgs);
    return avg;
}

float procedure_sendrecv_replace(float *num, int n, int local_rank, int comm_sz, MPI_Comm comm) {
    float temp = compute_avg(num, n);
    float *sub_avgs = (float *)malloc(sizeof(float) * comm_sz);
    int source = (local_rank + comm_sz - 1) % comm_sz;
    int dest = (local_rank + 1) % comm_sz;
    sub_avgs[0] = temp;
    for (int i = 1; i < comm_sz; i++) {
        MPI_Sendrecv_replace(&(sub_avgs[i]), 1, MPI_FLOAT, dest, 0, source, 0, comm, MPI_STATUS_IGNORE);
    }
    float avg = compute_avg(sub_avgs, comm_sz);
    free(sub_avgs);
    return avg;
}

float *create_rand_nums(int num_elements) {
    float *rand_nums = (float *)malloc(num_elements * sizeof(float));
    if (rand_nums == NULL) return NULL;
    for (int i = 0; i < num_elements; i++) {
        rand_nums[i] = (rand() / (float)RAND_MAX);
    }
    return rand_nums;
}

float compute_avg(float* nums, int num_elements) {
    float sum = 0.0;
    for (int i = 0; i < num_elements; i++) {
        sum += nums[i]; 
    }
    return sum / num_elements;
}