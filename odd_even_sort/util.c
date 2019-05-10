#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#define RAND_MAX_NUMBER 1000000
#define RAND_MAX_NUMBER_LEN 7

/*
    range: 0~RAND_MAX_NUMBER
*/
int *create_rand_array(int n) {
    srand(time(NULL));
    int *array = malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++) {
        array[i] = rand() % RAND_MAX_NUMBER;
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

int *read_rand_array_from_file(const char *filename, int *size) {
    FILE *fp;
    if ((fp = fopen(filename, "r")) == NULL) {
        return NULL;
    }
    char *temp = malloc(sizeof(char) * (RAND_MAX_NUMBER_LEN + 10));
    fgets(temp, RAND_MAX_NUMBER_LEN, fp);
    int n = atoi(temp);
    *size = n;
    int *array = malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++) {
        if (fgets(temp, RAND_MAX_NUMBER_LEN + 1, fp) != NULL) {
            array[i] = atoi(temp);
        } else {
            fclose(fp);
            return NULL;
        }
    }
    free(temp);
    fclose(fp);
    return array;
}
