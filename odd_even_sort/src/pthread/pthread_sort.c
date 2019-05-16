#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "../util/util.h"

#define COMPUTE_MAX_THREAD_COUNT(array_sz) \
    ((array_sz % 2 == 0) ? \
        (array_sz / 2) : \
        ((array_sz - 1) / 2))
#define COMPUTE_MAX_PAIR_IN_ARRAY(array_sz) COMPUTE_MAX_THREAD_COUNT(array_sz)

// arguments for sort
int thread_count = 0;
int *array_to_sort = NULL;
int array_to_sort_sz = 0;
// sort_internal
int max_pair_in_array = 0;

// variables for function barrier
pthread_mutex_t mutex_phase_sync;
pthread_cond_t cond_var_phase_sync;
int thread_counter = 0;

void barrier();
void *sort_internal(void *thread_no);
void *sort_internal_qsort(void *thread_no);

void set_thread_count(int count) {
    if (array_to_sort_sz == 0) {
        thread_count = count;
    } else {
        int max_thread_count = COMPUTE_MAX_THREAD_COUNT(array_to_sort_sz);
        thread_count = count > max_thread_count ? max_thread_count : count;
    }
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
    thread_counter = 0;

    pthread_t *thread_handles = malloc(sizeof(pthread_t) * thread_count);
    for (long thread_no = 0; thread_no < thread_count; thread_no++) {
        pthread_create(&thread_handles[thread_no], NULL, sort_internal_qsort, (void *)thread_no);
    }
    for (long thread_no = 0; thread_no < thread_count; thread_no++) {
        pthread_join(thread_handles[thread_no], NULL);
    }

    free(thread_handles);
    pthread_cond_destroy(&cond_var_phase_sync);
    pthread_mutex_destroy(&mutex_phase_sync);
}

void barrier() {
    pthread_mutex_lock(&mutex_phase_sync);
    thread_counter++;
    if (thread_counter == thread_count) {
        thread_counter = 0;
        pthread_cond_broadcast(&cond_var_phase_sync);
    } else {
        while (pthread_cond_wait(&cond_var_phase_sync, &mutex_phase_sync) != 0);
    }
    pthread_mutex_unlock(&mutex_phase_sync);
}

void *sort_internal(void *thread_no) {
    int local_rank = (long)thread_no;
    int local_num_pairs = max_pair_in_array / thread_count;
    if ((max_pair_in_array % thread_count != 0) && 
        (local_rank == thread_count - 1)) {
            local_num_pairs = max_pair_in_array - (local_num_pairs * (thread_count - 1));
    }
    int *local_addr = array_to_sort + local_rank * (max_pair_in_array / thread_count) * 2;
    
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
        barrier();
    }
}

// assume thread_count can divide size of array exactly
void *sort_internal_qsort(void *thread_no) {
    int local_rank = (long)thread_no;
    int local_num_keys = array_to_sort_sz / thread_count;
    int *local_addr = array_to_sort + local_rank * local_num_keys;

    qsort(local_addr, local_num_keys, sizeof(int), cmp);
    barrier();
    int *partner_keys = malloc(sizeof(int) * local_num_keys);
    int *temp = malloc(sizeof(int) * local_num_keys);
    for (int phase = 0; phase < thread_count; phase++) {
        int partner = compute_partner(phase, local_rank, thread_count);
        if (partner != -1) {
            copy_array(array_to_sort + partner * local_num_keys, partner_keys, local_num_keys);
            barrier();
            if (local_rank < partner) {
                merge_low(local_addr, partner_keys, temp, local_num_keys);
            } else {
                merge_high(local_addr, partner_keys, temp, local_num_keys);
            }
        } else {
            barrier();
        }
        barrier();
    }
    free(partner_keys);
    free(temp);
}
