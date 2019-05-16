#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "util.h"
#include "timer.h"

#define COMPUTE_MAX_THREAD_COUNT(array_sz) \
    ((array_sz % 2 == 0) ? \
        (array_sz / 2) : \
        ((array_sz - 1) / 2))
#define COMPUTE_MAX_PAIR_IN_ARRAY(array_sz) COMPUTE_MAX_THREAD_COUNT(array_sz)

int thread_count_global;
int *array_to_sort_global;
int max_pair_in_array_global;

void *sort_internal(void *thread_no) {
    int local_rank = (long)thread_no;
    int local_num_pairs;
    if ((max_pair_in_array_global % thread_count_global != 0) && 
        (local_rank == thread_count_global - 1)) {
            local_num_pairs = max_pair_in_array_global % thread_count_global;
    } else {
        local_num_pairs = max_pair_in_array_global / thread_count_global;
    }
    int *local_addr = array_to_sort_global + local_rank * local_num_pairs * 2;
    
}

void odd_even_sort_pthread() {
    pthread_t *thread_handles = malloc(sizeof(pthread_t) * thread_count_global);
    for (long thread_no = 0; thread_no < thread_count_global; thread_no++) {
        pthread_create(&thread_handles[thread_no], NULL, sort_internal, (void *)thread_no);
    }
    for (long thread_no = 0; thread_no < thread_count_global; thread_no++) {
        pthread_join(thread_handles[thread_no], NULL);
    }

    free(thread_handles);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: program <number_of_threads> <filename>\n");
        exit(1);
    }
    thread_count_global = atoi(argv[1]);
    char *filename = argv[2];
    int array_sz = 0;
    int *rand_array = read_rand_array_from_file(filename, &array_sz);
    if (rand_array == NULL) {
        printf("can't create random array from file %s\n", filename);
        return 0;
    }
    print_array(rand_array, array_sz);
    int max_thread_count = COMPUTE_MAX_THREAD_COUNT(array_sz);
    thread_count_global = thread_count_global > max_thread_count ? max_thread_count : thread_count_global;
    array_to_sort_global = rand_array;
    max_pair_in_array_global = COMPUTE_MAX_PAIR_IN_ARRAY(array_sz);

    double start, finish;
    GET_TIME(start);
    odd_even_sort_pthread();
    GET_TIME(finish);
    printf("%f\n", finish - start);

    return 0;
}
