#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc,char **argv)
{
 //int size = strtol(argv[1],NULL,10);
 //int cut = strtol(argv[2],NULL,10);
 int size,cut;
 scanf("%d %d",&size,&cut);
 int blocksize = size/cut;
 double **a = NULL,**b = NULL,**c = NULL;
 a = (double **)malloc(size * sizeof(double*));
 b = (double **)malloc(size * sizeof(double*));
 c = (double **)malloc(size * sizeof(double*));

 for(int i = 0;i < size;i++)
 {
  a[i] = (double*)malloc(sizeof(double)*size);
  b[i] = (double*)malloc(sizeof(double)*size);
  c[i] = (double*)malloc(sizeof(double)*size);
 }
 for(int i = 0;i < size;i++)
 {
  for(int j = 0;j < size;j++)
  {
   a[i][j] = 1.1;
   b[i][j] = 1.1;
  }
 }
 clock_t begin = clock();
 for(int m = 0;m < size;m += blocksize)
 {
  for(int l = 0;l < size;l += blocksize)
  {
   for(int i = 0;i < blocksize;i++)
   {
    for(int j = 0;j < blocksize;j++)
    {
     double total = 0;
     for(int k = 0;k < size;k++)
     {
      total += a[m+i][k] * b[k][l+j];
     }
     c[m+i][l+j] = total;
    }
   }
  }
 }
 clock_t end = clock();
 /*for(int i = 0;i < size;i++)
 {
  for(int j = 0;j < size;j++)
  {
   printf("%.2f ",c[i][j]);
  }
  printf("\n");
 }*/
 printf("%lf\n",((double)end - begin)/1000);

}
