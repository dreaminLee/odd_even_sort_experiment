#include "util.h"

void bubble_sort(int *array, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            if (array[j] > array[j+1]) {
                swap(&(array[j]), &(array[j+1]));
            }
        }
    }
}

void odd_even_sort(int *array, int n) {
    for (int phase = 0; phase < n; phase++) {
        if (phase % 2 == 0) {
            for (int i = 1; i < n; i += 2) {
                if (array[i-1] > array[i]) {
                    swap(&(array[i-1]), &(array[i]));
                }
            }
        } else {
            for (int i = 1; i < n-1; i += 2) {
                if (array[i] > array[i+1]) {
                    swap(&(array[i]), &(array[i+1]));
                }
            }
        }
    }
}