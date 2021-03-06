#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void omp_mm(float *A, float *B, float *C, int n)
{
    int i;
    int j;
    int k;
    float exec_time =0.0;
   
#pragma omp target map(to                                \
                       : A [0:n], B [0:n], n) map(tofrom \
                                                  : C [0:n])
    {
#pragma omp target teams distribute parallel for map(to                                                \
                                                     : A [0:n], B [0:n], n) shared(A, B, n) map(tofrom \
                                                                                                : C [0:n]) schedule(auto)

         clock_t start = clock();                                                                                    

        for (i = 0; i < n; i++)
        {
            for (j = 0; j < n; j++)
            {
                for (k = 0; k < n; k++)
                {
                    C[i * n + j] += A[i * n + k] * B[k * n + j];
                }
            }
        }

       clock_t end = clock();
        exec_time = ((float)(end - start)) / CLOCKS_PER_SEC;
      
          printf("Time taken is %f\n", exec_time);
    }
}

int main(int argc, char const *argv[])
{
    int i;
    int j;
    int k;

    int n = atoi(argv[1]);

    float *A, *B, *C;
    A = (float *)calloc(n * n, sizeof(float));
    B = (float *)calloc(n * n, sizeof(float));
    C = (float *)calloc(n * n, sizeof(float));

    for (i = 0; i < n; ++i)
    {
        for (j = 0; j < n; ++j)
        {
            A[i * n + j] = 1;
        }
    }

    for (i = 0; i < n; ++i)
    {
        for (j = 0; j < n; ++j)
        {
            B[i * n + j] = 2;
        }
    }

    omp_mm(A, B, C, n);

    for (i = 0; i < n; ++i)
    {
        for (j = 0; j < n; ++j)
        {
           printf("%f   ", C[i * n + j]);
        }
       printf("\n");
    }

}
