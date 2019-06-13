#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#define RAND_MAX_NUMBER 1000000
#define RAND_MAX_NUMBER_LEN 10

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

int compare_array(const int *array_a, const int *array_b, int array_sz) {
    for (int i = 0; i < array_sz; i++) {
        if (array_a[i] != array_b[i]) {
            return 0;
        }
    }
    return 1;
}

int cmp(const void *a, const void *b) {
    return (*((int *)a) - *((int *)b));
}

int compute_partner(int phase, int rank, int comm_size) {
    int partner;
    if (phase % 2 == 0) { // Even phase
        if (rank % 2 != 0) { // Odd rank
            partner = rank - 1;
        } else {            // Even rank
            partner = rank + 1;
        }
    } else {             // Odd phase
        if (rank % 2 != 0) { // Odd rank
            partner = rank + 1;
        } else {            // Even rank
            partner = rank - 1;
        }
    }
    if (partner == -1 || partner == comm_size) {
        partner = -1;
    }
    return partner;
}

void merge_low(int *local_keys, const int *recv_keys, int *temp_keys, int n) {
    int m_i, r_i, t_i;
    m_i = r_i = t_i = 0;

    while (t_i < n) {
        if (local_keys[m_i] <= recv_keys[r_i]) {
            temp_keys[t_i] = local_keys[m_i];
            m_i++;
        } else {
            temp_keys[t_i] = recv_keys[r_i];
            r_i++;
        }
        t_i++;
    }

    memcpy(local_keys, temp_keys, sizeof(int) * n);
}

void merge_high(int *local_keys, const int *recv_keys, int *temp_keys, int n) {
    int m_i, r_i, t_i;
    m_i = r_i = t_i = n - 1;

    while (t_i >= 0) {
        if (local_keys[m_i] >= recv_keys[r_i]) {
            temp_keys[t_i] = local_keys[m_i];
            m_i--;
        } else {
            temp_keys[t_i] = recv_keys[r_i];
            r_i--;
        }
        t_i--;
    }

    memcpy(local_keys, temp_keys, sizeof(int) * n);
}
