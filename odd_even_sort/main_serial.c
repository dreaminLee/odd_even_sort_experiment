#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "serial_sort.h"

int main() {
    int n;
    scanf("%d", &n);
    int *rand_array1 = create_rand_array(n);
    int *rand_array1_copy1 = create_array(n);
    copy_array(rand_array1, rand_array1_copy1, n);
    int *rand_array1_copy2 = create_array(n);
    copy_array(rand_array1, rand_array1_copy2, n);
    bubble_sort(rand_array1_copy1, n);
    odd_even_sort(rand_array1_copy2, n);
    print_array(rand_array1_copy1, n);
    print_array(rand_array1_copy2, n);
    free(rand_array1);
    free(rand_array1_copy1);
    free(rand_array1_copy2);
    return 0;
}
