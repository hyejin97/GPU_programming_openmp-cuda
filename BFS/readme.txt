Datasets:
  Input data is stored in data folder

Building Environment:
  gcc-11.2
  cuda-10.2

Compiling and executing the OpenMP version
  gcc-11.2

  gcc -fopenmp -Wall -std=c99 -lm -o bfsoff bfs.c
  ./bfsoff data/30000.txt

Compiling and executing the CUDA version
  gcc-4.8.5
  cuda-10.2

  nvcc -o bfs bfs.cu
  ./bfs data/30000.txt

Compiling and running the graph generator
  gcc-11.2
  g++ -std=c++11 -o graphGene graphGenerator.cpp
  ./graphGene num_of_vertices
