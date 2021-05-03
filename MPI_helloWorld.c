#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv){
    int size,id;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&id);
    printf("Hello World! I am %d of %d\n", id,size);

    if (id == 0){
        printf("hehexd\n");
    }


    MPI_Finalize();
}