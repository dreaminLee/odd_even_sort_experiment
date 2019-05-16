#include <stdio.h>
#include <stdlib.h>
#include "util.h"

/*
    create file data.txt containing random numbers.
    outputs a file "data.txt" containing randomly generated integers
    1 number per line.
    first line is the number of integers.
*/
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: program <size_of_array_to_be_sorted>\n");
        exit(1);
    }
    int n = atoi(argv[1]);
    int *rand_array = create_rand_array(n);
    FILE *fp = NULL;
    fp = fopen("./data.txt", "w");
    fprintf(fp, "%d\n", n);
    for (int i = 0; i < n; i++) {
        fprintf(fp, "%d\n", rand_array[i]);
    }
    fclose(fp);
    free(rand_array);

    return 0;
}
