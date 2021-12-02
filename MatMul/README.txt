Matrix multiplication using CUDA vs OpenMP:

Compiling and executing the OpenMP version:

  gcc-11.2

  gcc -o matrixMulmp -fopenmp -std=c99 matrixMul.c
  
  ./matrixMulmp [inputsize]

Compiling and executing the CUDA version:

  gcc-4.8.5
  cuda-10.2

  nvcc -o matrixMul matrixMul.cu
  
  ./matrixMul [inputsize]

