#include <stdio.h>
#include <stdlib.h>
#include "../util/util.h"
#include "../util/timer.h"
#include "serial_sort.h"
//#define DEBUG

/*
    output:
    <bubble_sort_time>
    <odd_even_sort_time>
*/
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: program <filename>\n");
        exit(1);
    }
    char *filename = argv[1];
    int n = 0;
    int *rand_array1 = read_rand_array_from_file(filename, &n);
    if (rand_array1 == NULL) {
        printf("can't create random array from file %s\n", filename);
        return 0;
    }
    int *rand_array1_copy1 = create_array(n);
    copy_array(rand_array1, rand_array1_copy1, n);
    int *rand_array1_copy2 = create_array(n);
    copy_array(rand_array1, rand_array1_copy2, n);
    int *rand_array1_copy3 = create_array(n);
    copy_array(rand_array1, rand_array1_copy3, n);

    double start, finish;
    GET_TIME(start);
    bubble_sort(rand_array1_copy1, n);
    GET_TIME(finish);
    printf("%f\n", finish - start);

    GET_TIME(start);
    odd_even_sort(rand_array1_copy2, n);
    GET_TIME(finish);
    printf("%f\n", finish - start);

    GET_TIME(start);
    quick_sort(rand_array1_copy3, n);
    GET_TIME(finish);
    printf("%f\n", finish - start);

    #ifdef DEBUG
    print_array(rand_array1_copy2, n);
    #endif

    free(rand_array1);
    free(rand_array1_copy1);
    free(rand_array1_copy2);
    free(rand_array1_copy3);
    return 0;
}
