#include <cuda.h>
#include <stdlib.h>
#include <stdio.h>

void matrixMultiplication(float *A, float *B, float *Result, int Width);
__global__ void matrixMulKernel(float *dA, float *dB, float *dResult, int Width);
int minimum(int, int);

int main(int argc, char *argv[])
{
    int N = atoi(argv[1]);

    float *A, *B, *Result;

    A = (float *)calloc(N * N, sizeof(float));
    B = (float *)calloc(N * N, sizeof(float));
    Result = (float *)calloc(N * N, sizeof(float));

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            A[i * N + j] = 1;
            B[i * N + j] = 2;
        }
    }

    matrixMultiplication(A, B, Result, N);

    free(A);
    free(B);
    free(Result);
}

void matrixMultiplication(float *A, float *B, float *Result, int Width)
{
    int size = sizeof(float) * Width * Width;
    float *dA, *dB, *dResult;

    cudaMalloc(&dA, size);
    cudaMemcpy(dA, A, size, cudaMemcpyHostToDevice);

    cudaMalloc(&dB, size);
    cudaMemcpy(dB, B, size, cudaMemcpyHostToDevice);

    cudaMalloc(&dResult, size);

    dim3 dimBlock(32, 32);
    dim3 dimGrid(ceil(double(Width + 31) / 32), ceil(double(Width + 31) / 32));
    if (Width > 10000)
    {
        dimGrid.x = ceil(double(Width + 31) / minimum(Width, 1000));
        dimGrid.y = ceil(double(Width + 31) / minimum(Width, 1000));
    }

    matrixMulKernel<<<dimGrid, dimBlock>>>(dA, dB, dResult, Width);

    cudaMemcpy(Result, dResult, size, cudaMemcpyDeviceToHost);

    for (int i = 0; i < Width; i++)
    {
        for (int j = 0; j < Width; j++)
        {
            printf("%f   ", Result[i * Width + j]);
        }
        printf("\n");
    }

    cudaFree(dA);
    cudaFree(dB);
    cudaFree(dResult);
}
int minimum(int n, int x)
{
    if (n < x)
    {
        return n;
    }
    else
    {
        return x;
    }
}
__global__ void matrixMulKernel(float *dA, float *dB, float *dResult, int width)
{

    int col = blockIdx.x * blockDim.x + threadIdx.x;
    int row = blockIdx.y * blockDim.y + threadIdx.y;

    int sum = 0;
    if (col < width && row < width)
    {
        for (int i = 0; i < width; i++)
        {
            sum += dA[row * width + i] * dB[i * width + col];
        }
        dResult[row * width + col] = sum;
    }
}
