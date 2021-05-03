#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

#define BLOCK_LOW(id,p,n) ((id)*(n)/(p))
#define BLOCK_HIGH(id,p,n) (BLOCK_LOW((id)+1,p,n)-1)
#define BLOCK_SIZE(id,p,n) (BLOCK_LOW((id)+1,p,n)-BLOCK_LOW((id),p,n))
#define BLOCK_OWNER(index,p,n) (((p)*((index)+1)-1)/(n))

double **matalloc(double **M,int n){
	M=(double **)malloc(sizeof(double*)*n);

	for(int i=0; i<n; i++){
		M[i]=(double*)malloc(sizeof(double)*n);
	}
	
	for(int i=0; i<n; i++){
		for(int j=0; j<n; j++){
		M[i][j]=0;
		}
	}
	
	return M;
}

void matfree(double **M, int n){
	for(int i=0;i<n;i++){
		free(M[i]);
	}
	free(M);
}

double *rowshift(double *M, int n, int shift){
	for(int i=0; i<shift; i++){
		double tmp = M[0];
		
		for(int j=1; j<n; j++)
			M[j-1] = M[j];
			
		M[n-1] = tmp;
	}
	
	return M;
}

double **colshift(double **M, int id, int p, int min, int size, int n, int index){
	MPI_Send(&M[0][index],1,MPI_DOUBLE,(id-1+p)%p,(min-1+n)%n,MPI_COMM_WORLD);
	
	for(int i=1; i<size; i++){
		M[i-1][index] = M[i][index];
	}
	
	MPI_Recv(&M[size-1][index],1,MPI_DOUBLE,(id+1)%p,(min+size-1),MPI_COMM_WORLD,NULL);
	
	return M;
}

int main(int argc, char *argv[]){
	int n = strtol(argv[1],NULL,10),id,p;
	MPI_Init(NULL,NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	int size = BLOCK_SIZE(id,p,n) ,min = BLOCK_LOW(id,p,n) ,max = BLOCK_HIGH(id,p,n);
	
	double **a = (double **)malloc(sizeof(double*)*size), **b = (double **)malloc(sizeof(double*)*size), **c = (double **)malloc(sizeof(double*)*size);
	double **A = matalloc(A,n), **B = matalloc(B,n), **C = matalloc(C,n);
	
	if(id==0){
		for(int i=0; i<n; i++)
			for(int j=0; j<n; j++)
				A[i][j]=1.1;
				
		for(int i=0; i<n; i++)
			for(int j=0; j<n; j++)
				B[i][j]=1.1;
				
		for(int i=0; i<n; i++){
			int tid = BLOCK_OWNER(i,p,n);
			if(tid==0){
				a[i] = (double *)malloc(sizeof(double)*n);
				b[i] = (double *)malloc(sizeof(double)*n);
				c[i-min] = (double *)malloc(sizeof(double)*n);
				
				for(int j=0; j<n; j++){
					a[i][j] = A[i][j];
					b[i][j] = B[i][j];
					c[i][j] = C[i][j];
				}
			}
			else{
				double *t = (double *)malloc(sizeof(double)*n*2);
				for(int j=0; j<n; j++){
					t[j] =A[i][j];
					t[n+j]=B[i][j];
				}
				
				MPI_Send(t,n*2,MPI_DOUBLE,tid,i,MPI_COMM_WORLD);
				free(t);
			}
		}
	}
	else{
		matfree(A,n);
		matfree(B,n);
		matfree(C,n);
		for(int i=min; i<(min+size); i++){
			double *t = (double *)malloc(sizeof(double)*n*2);
			MPI_Recv(t,n*2,MPI_DOUBLE,0,i,MPI_COMM_WORLD,NULL);
			a[i-min] = (double *)malloc(sizeof(double)*n);
			b[i-min] = (double *)malloc(sizeof(double)*n);
			c[i-min] = (double *)malloc(sizeof(double)*n);
			
			for(int j=0; j<n ;j++){
				a[i-min][j] = t[j];
				b[i-min][j] = t[n+j];
				c[i-min][j] = 0;
			}
		}
	}
	
	double t =MPI_Wtime(), time;
	
	for(int i=0; i<n; i++){
		if(id==BLOCK_OWNER(i,p,n)){
			for(int j=0; j<i;j++)
				a[i-min] = rowshift(a[i-min],n,i); 
		}
		for(int j=0; j<i; j++){
			MPI_Barrier(MPI_COMM_WORLD);
			b = colshift(b,id,p,min,size,n,i);
			MPI_Barrier(MPI_COMM_WORLD);
		}
	}
	MPI_Barrier(MPI_COMM_WORLD);
	
	for(int i=0; i<n; i++){
		for(int j=0; j<size;j++){
			for(int k=0; k<n; k++){
				c[j][k] += a[j][k]*b[j][k];
			}
			a[j] = rowshift(a[j],n,1);
		}
		for(int j=0; j<n; j++){
			MPI_Barrier(MPI_COMM_WORLD);
			b = colshift(b,id,p,min,size,n,j);
			MPI_Barrier(MPI_COMM_WORLD);
		}
	}
	t = MPI_Wtime()-t;
	
	if(id==0){
		for(int i=0;i<n;i++){
			int tid = BLOCK_OWNER(i,p,n);
			if(tid==0){
				for(int j=0; j<n; j++)
					C[i][j] = c[i][j];
			}
			else{
					double *t = (double *)malloc(sizeof(double)*n);
					MPI_Recv(t,n,MPI_DOUBLE,tid,i,MPI_COMM_WORLD,NULL);
					
					for(int j=0; j<n; j++)
						C[i][j] = t[j];
			}
		}
	}
	else{
		for(int i=0; i<size; i++){
			MPI_Send(c[i],n,MPI_DOUBLE,0,min+i,MPI_COMM_WORLD);
		}
	}
	
	for(int i=0; i<size; i++){
		free(a[i]);
		free(b[i]);
		free(c[i]);
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Reduce(&t,&time,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);
	printf("Process %d is done. Time%lf s\n",id,t);
	MPI_Finalize();
	if(id==0){
		for(int i=0; i<n ;i++){
			for(int j=0; j<n; j++)
				printf("%.2lf",C[i][j]);
			printf("\n");
		}
	}
	printf("Count: %.2f\n",C[0][0]);
	printf("Average time: %lf s\n",time/p);
	matfree(A,n);
	matfree(B,n);
	matfree(C,n);

return 0;
}
