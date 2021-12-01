#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <unistd.h>
#include <iostream>

#define ERR_NO_NUM -1
#define ERR_NO_MEM -2

int num_of_nodes;
//int num_of_edges;
int min_per_node_edge = 3;
int max_per_node_edge = 10;

int min_edge_length = 1;
int max_edge_length = 20;

//Structure to hold a node information
struct Node
{
std::vector<int> ends;
std::vector<int> weights;
};

std::vector<Node> nodes;

void swap(int* xp, int* yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

// Function to perform Selection Sort
void selectionSort(int arr[], int n)
{
    int i, j, min_idx;

    // One by one move boundary of unsorted subarray
    for (i = 0; i < n - 1; i++) {

        // Find the minimum element in unsorted array
        min_idx = i;
        for (j = i + 1; j < n; j++)
            if (arr[j] < arr[min_idx])
                min_idx = j;

        // Swap the found minimum element
        // with the first element
        swap(&arr[min_idx], &arr[i]);
    }
}

// Generates integers in range [lower, upper].
int Randoms(int lower, int upper)
{
  int num = (rand() % (upper - lower + 1)) + lower;
  return num;
}

int randomEnd (int lower, int upper, int *ppPool[]) {
    int i, n;

    // Initialize with a specific size.

    if (lower >= 0) {
        int size = upper - lower + 1;
        if (*ppPool != NULL)
            free (*ppPool);
        if ((*ppPool = (int*) malloc (sizeof(int) * (size+1))) == NULL)
            return ERR_NO_MEM;
        (*ppPool)[0] = size;
        for (i = 0; i < size; i++) {
            (*ppPool)[i+1] = i+lower;
        }
        return -1;
    }

    // Error if no numbers left in pool.
    if (*ppPool == NULL)
       return ERR_NO_NUM;

    // Get random number from pool and remove it (rnd in this
    //   case returns a number between 0 and numNums-1 inclusive).
    n = rand() % (*ppPool)[0];
    i = (*ppPool)[n+1];
    (*ppPool)[n+1] = (*ppPool)[(*ppPool)[0]];
    (*ppPool)[0]--;
    if ((*ppPool)[0] == 0) {
        free (*ppPool);
        *ppPool = NULL;
    }

    return i;
}

std::vector<int> sortE;
std::vector<int> sortW;
int compare (const void * a, const void * b) {
    double diff = sortE[*(int*)a] - sortE[*(int*)b];
    return  (0 < diff) - (diff < 0);
}

int main( int argc, char** argv)
{
	num_of_nodes=0;
  srand(time(NULL));

  /* Check input arguments. */
  if(argc != 2 ){
    printf("usage: ./randomGraph num_of_nodes\n");
    printf("num_of_nodes: number of nodes in the graph\n");
    exit(1);
  }
  num_of_nodes = atoi(argv[1]);
  if (max_per_node_edge > num_of_nodes) {
    max_per_node_edge = num_of_nodes - 1;
  }

  nodes.resize(num_of_nodes);
  int* pPool;
  for (int i=0; i<num_of_nodes-1; i++) {
    int max_e = nodes[i].ends.size() + num_of_nodes - 1 - i;
    int no_e;
    if (max_e < max_per_node_edge) {
      no_e = Randoms(min_per_node_edge, max_e);
    } else {
      no_e = Randoms(min_per_node_edge, max_per_node_edge);
    }
    int left = no_e - nodes[i].ends.size();
    pPool = NULL;
    int result;
    result = randomEnd(i+1, num_of_nodes-1, &pPool);
    for (int k=0; k<left; k++) {
      result = randomEnd(-1, -1, &pPool);
      nodes[i].ends.push_back(result);
      int rand = Randoms(min_edge_length, max_edge_length);
      nodes[i].weights.push_back(rand); // or simply push_back same value as ends to avoid sorting nightmare?
      nodes[result].ends.push_back(i);
      nodes[result].weights.push_back(rand);
    }
  }

  char filename[1024];
  FILE *f;
  sprintf(filename, "%d.txt", num_of_nodes);

  f = fopen(filename,"w");
  if (f == NULL)
  {
      printf("Error opening file!\n");
      exit(1);
  }
  fprintf(f, "%d\n", num_of_nodes);

  int cur_i = 0;
  for (int i = 0; i < num_of_nodes; i++) {
    int t = nodes[i].ends.size();
    // sort both vectors
    sortE = nodes[i].ends;
    sortW = nodes[i].weights;
    int perm[t], k;
    int resE[t];
    int resW[t];
    for (k = 0; k < t; k++) {
      perm[k] = k;
    }
    qsort(perm, t, sizeof(int), compare);
    for (k = 0; k < t; k++) {
      resE[k] = sortE[perm[k]];
      resW[k] = sortW[perm[k]];
    }
    for (int j=0; j<t; j++) {
      nodes[i].ends[j] = resE[j];
      nodes[i].weights[j] = resW[j];
    }

    fprintf(f, "%d %d\n", cur_i, t);
    cur_i += t;
  }
  // always start from 0
  fprintf(f, "%d\n", 0);

  fprintf(f, "%d\n", cur_i);
  for (int i = 0; i < num_of_nodes; i++) {
    // size of stored edges
    int t_s = nodes[i].ends.size();
    int tt_s = nodes[i].weights.size();
    if (t_s != tt_s) {
      std::cout << "Unmatch: " << i << std::endl;
    }
    for (int j = 0; j < t_s; j++) {
      fprintf(f, "%d %d\n", nodes[i].ends[j], nodes[i].weights[j]);
    }
  }
  fclose(f);
}
