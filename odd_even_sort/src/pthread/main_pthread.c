#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "../util/util.h"
#include "../util/timer.h"
#include "pthread_sort.h"
//#define DEBUG

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: program <number_of_threads> <filename>\n");
        exit(1);
    }
    set_thread_count(atoi(argv[1]));
    char *filename = argv[2];
    int rand_array_sz = 0;
    int *rand_array = read_rand_array_from_file(filename, &rand_array_sz);
    if (rand_array == NULL) {
        printf("can't create random array from file %s\n", filename);
        return 0;
    }
    #ifdef DEBUG
    print_array(rand_array, rand_array_sz);
    int *rand_array_copy = create_array(rand_array_sz);
    copy_array(rand_array, rand_array_copy, rand_array_sz);
    #endif
    set_array_to_sort(rand_array, rand_array_sz);

    double start, finish;
    GET_TIME(start);
    odd_even_sort_pthread();
    GET_TIME(finish);
    printf("%f\n", finish - start);
    #ifdef DEBUG
    print_array(rand_array, rand_array_sz);
    qsort(rand_array_copy, rand_array_sz, sizeof(int), cmp);
    printf("%d\n", compare_array(rand_array, rand_array_copy, rand_array_sz));
    #endif

    return 0;
}
