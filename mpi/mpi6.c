#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// Merges two subarrays of arr[]. 
// First subarray is arr[l..m] 
// Second subarray is arr[m+1..r] 
void merge(int arr[], int l, int m, int r) 
{ 
    int i, j, k; 
    int n1 = m - l + 1; 
    int n2 =  r - m; 
  
    /* create temp arrays */
    int L[n1], R[n2]; 
  
    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++) 
        L[i] = arr[l + i]; 
    for (j = 0; j < n2; j++) 
        R[j] = arr[m + 1+ j]; 
  
    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray 
    j = 0; // Initial index of second subarray 
    k = l; // Initial index of merged subarray 
    while (i < n1 && j < n2) 
    { 
        if (L[i] <= R[j]) 
        { 
            arr[k] = L[i]; 
            i++; 
        } 
        else
        { 
            arr[k] = R[j]; 
            j++; 
        } 
        k++; 
    } 
  
    /* Copy the remaining elements of L[], if there 
       are any */
    while (i < n1) 
    { 
        arr[k] = L[i]; 
        i++; 
        k++; 
    } 
  
    /* Copy the remaining elements of R[], if there 
       are any */
    while (j < n2) 
    { 
        arr[k] = R[j]; 
        j++; 
        k++; 
    } 
} 
  
/* l is for left index and r is right index of the 
   sub-array of arr to be sorted */
void mergeSort(int arr[], int l, int r) 
{ 
    if (l < r) 
    { 
        // Same as (l+r)/2, but avoids overflow for 
        // large l and h 
        int m = l+(r-l)/2; 
  
        // Sort first and second halves 
        mergeSort(arr, l, m); 
        mergeSort(arr, m+1, r); 
  
        merge(arr, l, m, r); 
    } 
}

void print_array(int *array, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
}

int main() {
    int local_rank, comm_sz;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &local_rank);
    srand(time(NULL) + local_rank);

    int n = 0;
    if (local_rank == 0) {
        scanf("%d", &n);
        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    } else {
        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }

    int sub_array_size = (local_rank == comm_sz - 1 && n % comm_sz != 0) ? n % comm_sz : n / comm_sz;
    int *sub_array = (int *)malloc(sizeof(int) * sub_array_size);
    for (int i = 0; i < sub_array_size; i++) {
        sub_array[i] = rand() % 1000;
    }

    mergeSort(sub_array, 0, sub_array_size - 1);
    if (local_rank == 0) {
        printf("Process 0: ");
        print_array(sub_array, sub_array_size);
        int *temp = (int *)malloc(sizeof(int) * sub_array_size);
        MPI_Status recv_status;
        for (int i = 1; i < comm_sz; i++) {
            printf("Process %d: ", i);
            MPI_Recv(temp, sub_array_size, MPI_INT, i, 0, MPI_COMM_WORLD, &recv_status);
            int recv_count;
            MPI_Get_count(&recv_status, MPI_INT, &recv_count);
            print_array(temp, recv_count);
        }
    } else {
        MPI_Send(sub_array, sub_array_size, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    

    MPI_Finalize();
    return 0;
}
