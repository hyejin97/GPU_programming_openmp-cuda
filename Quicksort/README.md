Datasets:
  genexamples.c produces input data for quicksort applications
  type this command on terminal
  "
  gcc -o genexample genexamples.c -std=c99
  "
  and run the executable with an argument n (the length of array)
  "
  ./genexample 1000
  "
  it creates input file "1000in.txt"
  
Compiling and executing the OpenMP version
  gcc-11.2

  gcc -fopenmp -std=c99 quicksort.c
  ./a.out 1000in.txt

Compiling and executing the CUDA version
  gcc-4.8.5
  cuda-10.2

  nvcc -arch=sm_35 -rdc=true quicksort.cu -o cudaqsort -lcudadevrt
  ./cudaqsort 1000in.txt

Both Cuda version and OpenMP version of program outputs the sorted array in an output file "n.txt" (ex. 1000.txt)
