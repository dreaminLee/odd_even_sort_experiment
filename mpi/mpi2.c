#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

int *prefix_sum_serial(int *, int);
int *prefix_sum_parallel(int *, int);
void prefix_sum_parallel_mpi(int *, int, int **);
int *create_rand_nums(int);
void print_nums(int *, int);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        exit(1);
    }
    int num_elements = atoi(argv[1]);
    /*
    MPI_Init(&argc, &argv);
    int local_rank, comm_sz;
    MPI_Comm_rank(MPI_COMM_WORLD, &local_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);



    MPI_Finalize();
    */
    srand(time(NULL));
    int *nums = create_rand_nums(num_elements);
    //print_nums(nums, num_elements);
    //print_nums(prefix_sum_serial(nums, num_elements), num_elements);
    //print_nums(prefix_sum_parallel(nums, num_elements), num_elements);
    int *prefix_sum = (int *)malloc(sizeof(int) * num_elements);
    prefix_sum_parallel_mpi(nums, num_elements, &prefix_sum);
    print_nums(prefix_sum, num_elements);
    return 0;
}

int *prefix_sum_serial(int *nums, int num_elements) {
    int *prefix_sum = (int *)malloc(sizeof(int) * num_elements);
    for (int i = 0; i < num_elements; i++) {
        prefix_sum[i] = nums[i];
    }
    for (int i = 1; i < num_elements; i++) {
        prefix_sum[i] += prefix_sum[i-1];
    }
    return prefix_sum;
}

int *prefix_sum_parallel(int *nums, int num_elements) {
    int *prefix_sum[2] = {(int *)malloc(sizeof(int) * num_elements), 
                          (int *)malloc(sizeof(int) * num_elements)};
    for (int i = 0; i < num_elements; i++) {
        prefix_sum[0][i] = nums[i];
    }
    int num_phase = (int)ceil(log2((double)num_elements));
    for (int i = 0; i < num_phase; i++) {
        int new_index = (i + 1) % 2;
        int old_index = i % 2;
        int gap = exp2(i);
        for (int j = 0; j < num_elements; j++) {
            if (j < gap) {
                prefix_sum[new_index][j] = prefix_sum[old_index][j];
            } else {
                prefix_sum[new_index][j] = prefix_sum[old_index][j-gap] + prefix_sum[old_index][j];
            }
        }
        //print_nums(prefix_sum[new_index], num_elements);
    }
    free(prefix_sum[(num_phase + 1) % 2]);
    return prefix_sum[num_phase % 2];
}

void prefix_sum_parallel_mpi(int *origin_nums, int num_elements, int **prefix_sum) {
    int local_rank, comm_sz;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &local_rank);
    int *nums = NULL;
    if (local_rank == 0) {
        //print_nums(origin_nums, num_elements);
        nums = (int *)malloc(sizeof(int) * num_elements);
        for (int i = 0; i < num_elements; i++) {
            nums[i] = origin_nums[i];
        }
        //print_nums(prefix_sum_parallel(nums, num_elements), num_elements);
    }
    int local_num_elements = num_elements;

    int num_elements_per_proc = num_elements / comm_sz;
    int *sub_prefix_sum[2] = {(int *)malloc(sizeof(int) * num_elements_per_proc),
                              (int *)malloc(sizeof(int) * num_elements_per_proc)};
    MPI_Scatter(nums, 
                num_elements_per_proc, 
                MPI_INT, 
                sub_prefix_sum[0], 
                num_elements_per_proc, 
                MPI_INT, 
                0, MPI_COMM_WORLD);

    int num_phase = (int)ceil(log2((double)num_elements));
    int local_max_index_in_global, local_min_index_in_global;
    local_min_index_in_global = local_rank * num_elements_per_proc;
    local_max_index_in_global = local_min_index_in_global + num_elements_per_proc - 1;
    for (int i = 0; i < num_phase; i++) {
        int new_index = (i + 1) % 2;
        int old_index = i % 2;
        int send_count = 0;
        int recv_count = 0;
        int gap = exp2(i);
        // send data to other process
        if (local_max_index_in_global + gap < num_elements) {
            int dest = floor((local_max_index_in_global + gap) / num_elements_per_proc);
            for (int j = 0; j < num_elements_per_proc; j++) {
                if (j + gap >= num_elements_per_proc) {
                    //printf("send index %d to %d\n", j + local_min_index_in_global, local_rank + 1);
                    MPI_Send(&(sub_prefix_sum[old_index][j]), 1, MPI_INT, 
                            dest, send_count++, 
                            MPI_COMM_WORLD);
                }
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
        //printf("entering op phase in %d\n", local_rank);
        for (int j = 0; j < num_elements_per_proc; j++) {
            if (j + local_min_index_in_global < gap) {
                //printf("index %d unchanged\n", j + local_min_index_in_global);
                sub_prefix_sum[new_index][j] = sub_prefix_sum[old_index][j];
            } else {
                if (j - gap >= 0) {
                    //printf("index %d changed using local data\n", j + local_min_index_in_global);
                    sub_prefix_sum[new_index][j] = sub_prefix_sum[old_index][j] + 
                                                   sub_prefix_sum[old_index][j-gap];
                } else {
                    // recv data from other process
                    int temp_sum;
                    int source = floor((j + local_min_index_in_global - gap) / num_elements_per_proc);
                    MPI_Recv(&temp_sum, 1, MPI_INT, 
                             source, recv_count++, 
                             MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    sub_prefix_sum[new_index][j] = sub_prefix_sum[old_index][j] + temp_sum;
                    //printf("recv index %d from %d\n", j + local_min_index_in_global - gap, local_rank - 1);
                }
            }
        }

        /*
        int *temp_prefix_sum = NULL;
        if (local_rank == 0) {
            temp_prefix_sum = (int *)malloc(sizeof(int) * num_elements);
        }
        MPI_Gather(sub_prefix_sum[new_index], num_elements_per_proc, MPI_INT,
                   temp_prefix_sum, num_elements_per_proc, MPI_INT,
                   0, MPI_COMM_WORLD);
        if (local_rank == 0) {
            //printf("phase %d: ", i);
            print_nums(temp_prefix_sum, num_elements);
            free(temp_prefix_sum);
        }
        */

        MPI_Barrier(MPI_COMM_WORLD);
    }
    free(sub_prefix_sum[(num_phase + 1) % 2]);
    MPI_Gather(sub_prefix_sum[num_phase % 2], num_elements_per_proc, MPI_INT, 
               nums, num_elements_per_proc, MPI_INT, 
               0, MPI_COMM_WORLD);
    free(sub_prefix_sum[num_phase % 2]);
    if (local_rank == 0) {
        for (int i = 0; i < num_elements; i++) {
            (*prefix_sum)[i] = nums[i];
        }
        free(nums);
    }
    
    MPI_Finalize();
}

int *create_rand_nums(int num_elements) {
    int *rand_nums = (int *)malloc(num_elements * sizeof(int));
    if (rand_nums == NULL) return NULL;
    for (int i = 0; i < num_elements; i++) {
        rand_nums[i] = rand() % 100;
    }
    return rand_nums;
}

void print_nums(int *nums, int num_elements) {
    for (int i = 0; i < num_elements; i++) {
        printf("%d ", nums[i]);
    }
    printf("\n");
}