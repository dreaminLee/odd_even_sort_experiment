#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "pthread_sort.h"
#include "util.h"
#include "timer.h"

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
    #endif
    set_array_to_sort(rand_array, rand_array_sz);

    double start, finish;
    GET_TIME(start);
    odd_even_sort_pthread();
    GET_TIME(finish);
    printf("%f\n", finish - start);

    return 0;
}
