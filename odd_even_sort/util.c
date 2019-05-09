#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

int *create_rand_array(int n) {
    srand(time(NULL));
    int *array = malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++) {
        array[i] = rand() % 1000000;
    }
    return array;
}

int *create_array(int n) {
    return (int *)malloc(sizeof(int) * n);
}

void copy_array(const int *src, int *dest, int n) {
    memcpy(dest, src, sizeof(int) * n);
}

void print_array(const int *array, int n) {
    for (int i = 0; i < n; i++) {
        printf("%d\n", array[i]);
    }
    printf("\n");
}

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}
