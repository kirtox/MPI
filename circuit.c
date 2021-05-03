#include <mpi.h>
#include <stdio.h>
#include <time.h>
#define EXTRACT_BIT(n, i) ( ( n & (1 << i) ) ? 1 : 0 )

int main(int argc, char *argv[]){
    clock_t start = clock();
    int id;
    int size;
    int global_solutions;
    int solutions;
    int check_circuit(int, int);

    // Parallel computing Start
    MPI_Init(&argc, &argv);

    // MPI_COMM_WORLD denote all of the involved CPUs
    // Total number of CPUs can participate
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    // ID of CPUs
    MPI_Comm_rank(MPI_COMM_WORLD, &id);


    solutions = 0;
    for (int i = id ; i<65535 ; i += size){
        solutions += check_circuit(id, i);
        //printf("Process %d is finished!\n", id);
    }

    // Collect results of other CPUs to CPU ID:0
    MPI_Reduce(&solutions, &global_solutions, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    
    fflush(stdout);

    // Parallel computing end
    MPI_Finalize();

    clock_t end = clock();
    float seconds = (float)(end-start) / CLOCKS_PER_SEC;

    if (id == 0){
        printf("There are %d different solutions\n", global_solutions);
        printf("Period: %f\n", seconds);
    }
    
    return 0;
}

int check_circuit(int id, int z){
    int v[16];

    for (int i=0 ; i<16 ; i++){
        v[i] = EXTRACT_BIT(z, i);
    }

    // Find all of the solutions that the answer equals 1
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

        printf("%d) %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\n", id, 
        v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8],
        v[9], v[10], v[11], v[12], v[13], v[14], v[15]);

        // Enforce to output the stdout
        fflush(stdout);
        return 1;
    }
    fflush(stdout);
    return 0;
}