#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "../util/util.h"

#define COMPUTE_MAX_THREAD_COUNT(array_sz) \
    ((array_sz % 2 == 0) ? \
        (array_sz / 2) : \
        ((array_sz - 1) / 2))
#define COMPUTE_MAX_PAIR_IN_ARRAY(array_sz) COMPUTE_MAX_THREAD_COUNT(array_sz)

int thread_count = 0;
int *array_to_sort = NULL;
int array_to_sort_sz = 0;
int max_pair_in_array = 0;

pthread_mutex_t mutex_phase_sync;
pthread_cond_t cond_var_phase_sync;

void *sort_internal(void *thread_no);

void set_thread_count(int thread_count) {
    thread_count = thread_count;
}

void set_array_to_sort(int *array, int array_sz) {
    array_to_sort = array;
    array_to_sort_sz = array_sz;
    max_pair_in_array = COMPUTE_MAX_PAIR_IN_ARRAY(array_sz);
    int max_thread_count = COMPUTE_MAX_THREAD_COUNT(array_sz);
    thread_count = thread_count > max_thread_count ? max_thread_count : thread_count;
}

void odd_even_sort_pthread() {
    // init conditional variable and mutex for sync
    pthread_mutex_init(&mutex_phase_sync, NULL);
    pthread_cond_init(&cond_var_phase_sync, NULL);

    pthread_t *thread_handles = malloc(sizeof(pthread_t) * thread_count);
    for (long thread_no = 0; thread_no < thread_count; thread_no++) {
        pthread_create(&thread_handles[thread_no], NULL, sort_internal, (void *)thread_no);
    }
    for (long thread_no = 0; thread_no < thread_count; thread_no++) {
        pthread_join(thread_handles[thread_no], NULL);
    }

    free(thread_handles);
    pthread_cond_destroy(&cond_var_phase_sync);
    pthread_mutex_destroy(&mutex_phase_sync);
}

void *sort_internal(void *thread_no) {
    int local_rank = (long)thread_no;
    int local_num_pairs;
    if ((max_pair_in_array % thread_count != 0) && 
        (local_rank == thread_count - 1)) {
            local_num_pairs = max_pair_in_array % thread_count;
    } else {
        local_num_pairs = max_pair_in_array / thread_count;
    }
    int *local_addr = array_to_sort + local_rank * local_num_pairs * 2;
    
    // sort start
    for (int phase = 0; phase < array_to_sort_sz; phase++) {
        int *cur_addr = local_addr + phase % 2;
        int cur_num_pairs = local_num_pairs - (phase % 2 == 1 && local_rank == thread_count - 1);
        // swap
        for (int i = 0; i < cur_num_pairs; i++) {
            if (cur_addr[0] > cur_addr[1]) {
                swap(cur_addr, cur_addr + 1);
            }
            cur_addr += 2;
        }
        // sync: wait for other threads to finish

    }
}