#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>
#include "timer.h"

/*
    a. private(i, count, j) shared(temp, a, n)
    b. Loop dependency does not exist.
    c. memcpy can be parallel
*/

void count_sort(int *a, int n) {
    int i, j, count;
    int *temp = malloc(sizeof(int) * n);
    for (i = 0; i < n; i++) {
        count = 0;
        for (j = 0; j < n; j++) {
            if (a[j] < a[i]) {
                count++;
            } else if (a[j] == a[i] && j < i) {
                count++;
            }
        }
        temp[count] = a[i];
    }
    memcpy(a, temp, sizeof(int) * n);
    free(temp);
}

void count_sort_openmp(int *a, int n, int thread_counts) {
    int i, j, count;
    int *temp = malloc(sizeof(int) * n);
#   pragma omp parallel for num_threads(thread_counts) default(none) private(i, count, j) shared(temp, a, n)
    for (i = 0; i < n; i++) {
        count = 0;
        for (j = 0; j < n; j++) {
            if (a[j] < a[i]) {
                count++;
            } else if (a[j] == a[i] && j < i) {
                count++;
            }
        }
        temp[count] = a[i];
    }
    memcpy(a, temp, sizeof(int) * n);
    free(temp);
}

int cmp(const void *a, const void *b) {
    return (*((int *)a) - *((int *)b));
}

void print_array(int *a, int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");
}

int *create_rand_nums(int num_elements) {
    int *rand_nums = (int *)malloc(num_elements * sizeof(int));
    if (rand_nums == NULL) return NULL;
    for (int i = 0; i < num_elements; i++) {
        rand_nums[i] = rand();
    }
    return rand_nums;
}

int main(int argc, char *argv[]) {
    int n, thread_counts;
    printf("Input the size of random array: ");
    scanf("%d", &n);
    printf("Input the number of threads for openmp: ");
    scanf("%d", &thread_counts);
    srand(time(NULL));
    int *rand_nums = create_rand_nums(n);
    int *rand_nums_copy = malloc(sizeof(int) * n);
    int *rand_nums_copy2 = malloc(sizeof(int) * n);
    memcpy(rand_nums_copy, rand_nums, sizeof(int) * n);
    memcpy(rand_nums_copy2, rand_nums, sizeof(int) * n);

    double start, finish;
    GET_TIME(start);
    count_sort(rand_nums_copy, n);
    GET_TIME(finish)
    printf("serial count_sort:               %f\n", finish - start);
    GET_TIME(start);
    count_sort_openmp(rand_nums, n, thread_counts);
    GET_TIME(finish)
    printf("open_mp count_sort (%d threads):  %f\n", thread_counts, finish - start);
    GET_TIME(start);
    qsort(rand_nums_copy2, n, sizeof(int), cmp);
    GET_TIME(finish);
    printf("qsort:                           %f\n", finish - start);

    free(rand_nums);
    free(rand_nums_copy);
    free(rand_nums_copy2);
    return 0;
}
