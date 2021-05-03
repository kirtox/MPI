#include <mpi.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    clock_t start = clock();
    int count=0, id;
    // Parallel computing closed
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    printf("Process %d is online\n", id);
    for (int i = 0 ; i<65535 ; i ++){
        int tmp = i;
        bool *v=(bool *)malloc(sizeof(bool)*16);
        for (int j=0 ; j<16 ; j++){
            v[j] = (tmp%2);
            tmp/=2;
        }

        if (
            (v[0] || v[1]) && (!v[1] || !v[3]) && 
            (v[2] || v[3]) && (!v[3] || !v[4]) && 
            (v[4] || !v[5]) && (v[5] || !v[6])  && 
            (v[5] || v[6]) && (v[6] || !v[15]) && 
            (v[7] || !v[8]) && (!v[7] || !v[13]) && 
            (v[8] || v[9]) && (v[8] || !v[9]) && 
            (!v[9] || !v[10]) && (v[9] || v[11]) && 
            (v[10] || v[11]) && (v[12] || v[13]) && 
            (v[13] || !v[14]) && (v[14] || v[15])
            ){
            count++;
            
            printf("%d)", id);
            for (int j=0 ; j<16 ; j++){
                printf("%d", v[j]);
            }
            printf("\n");
        }
    }
    clock_t end = clock();
    float seconds = (float)(end-start) / CLOCKS_PER_SEC;
    printf("There are %d different solutions\n", count);
    printf("Period: %f\n", seconds);
    MPI_Finalize();
    return 0;
}
