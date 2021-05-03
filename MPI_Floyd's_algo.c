#include <stdio.h>
#include <time.h>
#include <mpi.h>
#define MAX 5
#define inf 0x3f3f3f3f

int size, id;
int ans[MAX][MAX];

int min(int a, int b){
    if (a<b && a>=0){
        return a;
    } else {
        return b;
    }
}

void print(int *x){
    if (id == 0){
        for (int i=0 ; i<MAX ; i++){
            for (int j=0 ; j<MAX ; j++){
                if (*(x + MAX*i + j) == inf){
                    printf("* ");
                } else {
                    printf("%d ", *(x + MAX*i + j));
                }
            }
            printf("\n");
        }
    }
}

int main(int argc, char *argv[]){
    // Parallel computing Start
    MPI_Init(&argc, &argv);

    // MPI_COMM_WORLD denote all of the involved CPUs
    // Total number of CPUs can participate
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    // ID of CPUs
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    // data initialization
    int dis[MAX][MAX] = {
        {0, 2, inf, inf, inf},
        {inf, 0, 3, inf, inf},
        {inf, inf, 0, 4, 1},
        {5, inf, inf, 0, inf},
        {6, inf, inf, inf, 0}
    };
    // initial matrix
    print(&dis[0][0]);
    printf("\n");

    clock_t start = clock();

    for (int k=0 ; k<MAX ; k++){
        // broadcast k%size process row k datas to other processes
        MPI_Bcast(dis[k], MAX, MPI_INT, k%size, MPI_COMM_WORLD);

        for (int i=id ; i<MAX ; i+=size){
            for (int j=0 ; j<MAX ; j++){
                dis[i][j] = min(dis[i][k] + dis[k][j], dis[i][j]);
            }
        }
        
    }

    MPI_Reduce(dis, ans, MAX*MAX, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
    clock_t end = clock();

    // print(&ans[0][0]);
    if (id == 0){
        for (int i=0 ; i<5 ; i++){
            for (int j=0 ; j<5 ; j++){
                printf("%2d", ans[i][j]);
                printf(" ");
            }
            printf("\n");
        }
    }

    printf("CPU %d => time: %lf\n", id, (end-start)/(double)(CLOCKS_PER_SEC));
    MPI_Finalize();
}