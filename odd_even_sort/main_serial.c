#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "serial_sort.h"
#include "timer.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: program <size_of_array_to_be_sorted>\n");
        exit(1);
    }
    int n = atoi(argv[1]);
    int *rand_array1 = create_rand_array(n);
    int *rand_array1_copy1 = create_array(n);
    copy_array(rand_array1, rand_array1_copy1, n);
    int *rand_array1_copy2 = create_array(n);
    copy_array(rand_array1, rand_array1_copy2, n);

    double start, finish;
    GET_TIME(start);
    bubble_sort(rand_array1_copy1, n);
    GET_TIME(finish);
    printf("%f\n", finish - start);

    GET_TIME(start);
    odd_even_sort(rand_array1_copy2, n);
    GET_TIME(finish);
    printf("%f\n", finish - start);

    free(rand_array1);
    free(rand_array1_copy1);
    free(rand_array1_copy2);
    return 0;
}
