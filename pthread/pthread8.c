#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

typedef long long ll;

typedef struct Thread_arguments {
    ll num_of_tosses;
    long rank;
} thread_arguments;

void *pi_estimation(void *arg) {
    ll *num_in_circle = malloc(sizeof(ll));
    ll num_of_tosses = ((thread_arguments *)arg)->num_of_tosses;
    long rank = ((thread_arguments *)arg)->rank;
    unsigned int rand_seed = time(NULL) ^ rank;
    for (ll toss = 0; toss < num_of_tosses; toss++) {
        double x = ((double)rand_r(&rand_seed) / RAND_MAX) * 2 - 1;
        double y = ((double)rand_r(&rand_seed) / RAND_MAX) * 2 - 1;
        double distance_squared = x*x + y*y;
        if (distance_squared <= 1) {
            (*num_in_circle)++;
        }
    }
    return num_in_circle;
}

int main(int argc, char *argv[]) {
    long thread;
    int thread_count;
    pthread_t *thread_handles;

    if (argc == 2) {
        thread_count = atoi(argv[1]);
    } else {
        thread_count = 2;
    }
    thread_handles = malloc(thread_count * sizeof(pthread_t));

    ll total_num_of_tosses;
    scanf("%lld", &total_num_of_tosses);
    thread_arguments *args;
    ll num_of_tosses_per_proc = total_num_of_tosses / thread_count;

    for (thread = 0; thread < thread_count; thread++) {
        args = malloc(sizeof(thread_arguments));
        args->num_of_tosses = num_of_tosses_per_proc;
        args->rank = thread;
        pthread_create(&thread_handles[thread], NULL, pi_estimation, (void *)args);
    }

    ll total_num_in_circle = 0;
    void *temp = malloc(sizeof(ll));
    for (thread = 0; thread < thread_count; thread++) {
        pthread_join(thread_handles[thread], &temp);
        total_num_in_circle += (*(ll *)temp);
    }
    free(temp);
    free(thread_handles);

    printf("%e\n", 4 * total_num_in_circle / (double)total_num_of_tosses);

    return 0;
}
