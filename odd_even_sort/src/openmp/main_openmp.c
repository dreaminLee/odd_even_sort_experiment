#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include "../util/util.h"
#include "../util/timer.h"
//#define DEBUG

#define SWAP(a, b) \
    int t = a; \
    a = b; \
    b = t

void odd_even_sort_openmp(int *array, int array_sz);

int thread_count;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: program <number_of_threads> <filename>\n");
        exit(1);
    }
    thread_count = atoi(argv[1]);
    char *filename = argv[2];
    int n = 0;
    int *rand_array1 = read_rand_array_from_file(filename, &n);
    if (rand_array1 == NULL) {
        printf("can't create random array from file %s\n", filename);
        return 0;
    }
    #ifdef DEBUG
    int *rand_array1_copy = create_array(n);
    copy_array(rand_array1, rand_array1_copy, n);
    #endif

    double start, finish;
    GET_TIME(start);
    odd_even_sort_openmp(rand_array1, n);
    GET_TIME(finish);
    printf("%f\n", finish - start);
    #ifdef DEBUG
    qsort(rand_array1_copy, n, sizeof(int), cmp);
    printf("%d\n", compare_array(rand_array1, rand_array1_copy, n));
    #endif

    free(rand_array1);
    return 0;
}

void odd_even_sort_openmp(int *array, int n) {
    int i, phase;
#   pragma omp parallel num_threads(thread_count) \
    default(none) shared(array, n) private(i, phase)
    for (phase = 0; phase < n; phase++) {
        if (phase % 2 == 0) {
#           pragma omp for schedule(runtime)
            for (i = 1; i < n; i += 2) {
                if (array[i-1] > array[i]) {
                   SWAP(array[i-1], array[i]);
                }
            }
        } else {
#           pragma omp for schedule(runtime)
            for (i = 1; i < n-1; i += 2) {
                if (array[i] > array[i+1]) {
                   SWAP(array[i+1], array[i]);
                }
            }
        }
    }
}
