#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <stdbool.h>

#define MAX_THREADS_PER_BLOCK 256

int no_of_nodes;
int edge_list_size;
FILE *fp;

int num_of_teams;
int num_of_threads_per_block;

//Structure to hold a node information
typedef struct Node
{
int starting;
int no_of_edges;
} Node;

void run_bfs_gpu(int no_of_nodes, Node *d_graph_nodes, int edge_list_size,
    int *d_graph_edges, bool *d_graph_mask, bool *d_updating_graph_mask, bool *d_graph_visited, int *d_cost);

void BFSGraph(int argc, char** argv);

////////////////////////////////////////////////////////////////////////////////
// Main Program
////////////////////////////////////////////////////////////////////////////////
int main( int argc, char** argv)
{
	no_of_nodes=0;
	edge_list_size=0;
    BFSGraph( argc, argv);
    //CUT_EXIT(argc, argv);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//Apply BFS on a Graph using OpenMp offloading
////////////////////////////////////////////////////////////////////////////////
void BFSGraph( int argc, char** argv)
{
	printf("Reading File\n");
	const char *input_file_name;

	if (argc == 2 ) {
		input_file_name = argv[1];
		printf("Input file: %s\n", input_file_name);
	}
	//Read in Graph from a file
	fp = fopen(input_file_name,"r");
	if(!fp)
	{
	printf("Error Reading graph file\n");
	return;
	}

	int source = 0;

	fscanf(fp,"%d",&no_of_nodes);

	num_of_teams = 1;
	num_of_threads_per_block = no_of_nodes;

	//Make execution Parameters according to the number of nodes
	//Distribute threads across multiple Blocks if necessary
	if(no_of_nodes>MAX_THREADS_PER_BLOCK)
	{
	   num_of_teams = (int)ceil(no_of_nodes/(double)MAX_THREADS_PER_BLOCK);
	   num_of_threads_per_block = MAX_THREADS_PER_BLOCK;
	}

	// allocate host memory
    Node* h_graph_nodes = (Node*) malloc(sizeof(Node)*no_of_nodes);
    bool *h_graph_mask = (bool*) malloc(sizeof(bool)*no_of_nodes);
    bool *h_graph_visited = (bool*) malloc(sizeof(bool)*no_of_nodes);

    bool *h_updating_graph_mask = (bool*) malloc(sizeof(char)*no_of_nodes);

    int start, edgeno;
    // initalize the memory
    for( unsigned int i = 0; i < no_of_nodes; i++)
    {
  		  fscanf(fp,"%d %d",&start,&edgeno);
		    h_graph_nodes[i].starting = start;
        h_graph_nodes[i].no_of_edges = edgeno;
        h_graph_mask[i]=false;
        h_graph_visited[i]=false;

        h_updating_graph_mask[i]=false;
    }

    //read the source node from the file
    fscanf(fp,"%d",&source);

    //set the source node as true in the mask
    h_graph_mask[source]=true;

		h_graph_visited[source] = true;

    fscanf(fp,"%d",&edge_list_size);

    int id,cost;
    int* h_graph_edges = (int*) malloc(sizeof(int)*edge_list_size);
    for(int i=0; i < edge_list_size ; i++)
    {
		fscanf(fp,"%d",&id);
		fscanf(fp,"%d",&cost);
		h_graph_edges[i] = id;
    }

	if(fp)
	fclose(fp);

	printf("Read File\n");

    // allocate mem for the result on host side
	int* h_cost = (int*) malloc( sizeof(int)*no_of_nodes);
	for(int i=0;i<no_of_nodes;i++) {
			h_cost[i]=-1;
	}
	h_cost[source]=0;

	printf("Copied Everything to GPU memory\n");

    // setup execution parameters
  run_bfs_gpu(no_of_nodes,h_graph_nodes,edge_list_size,h_graph_edges,
       h_graph_mask, h_updating_graph_mask, h_graph_visited, h_cost);

  //int k=0;


	//Store the result into a file
	FILE *fpo = fopen("resultoff.txt","w");
	for(int i=0;i<no_of_nodes;i++)
	fprintf(fpo,"%d) cost:%d\n",i,h_cost[i]);
	fclose(fpo);
	printf("Result stored in resultoff.txt\n");


    // cleanup memory
  free( h_graph_nodes);
  free( h_graph_edges);
  free( h_graph_mask);
	free( h_updating_graph_mask);
  free( h_graph_visited);
  free( h_cost);
}

void run_bfs_gpu(int no_of_nodes, Node *d_graph_nodes, int edge_list_size,
    int *d_graph_edges, bool *d_graph_mask, bool *d_updating_graph_mask, bool *d_graph_visited, int *d_cost)
{
  //make a variable to check if the execution is over
  char d_over[1];

#pragma omp target data map(to: d_graph_nodes[0:no_of_nodes],d_graph_edges[0:edge_list_size],d_graph_visited[0:no_of_nodes],d_graph_mask[0:no_of_nodes],d_updating_graph_mask[0:no_of_nodes]) map(alloc: d_over[0:1]) map(tofrom: d_cost[0:no_of_nodes])
  {
    do {
      d_over[0] = 0;
#pragma omp target update to (d_over[0:1])

#pragma omp target teams distribute parallel for num_teams(num_of_teams) thread_limit(num_of_threads_per_block)
      for (int tid = 0; tid < no_of_nodes; tid++) {
        if(d_graph_mask[tid]){
          d_graph_mask[tid]=false;
					//d_graph_visited[tid]=true;
          for(int i=d_graph_nodes[tid].starting;
						i<(d_graph_nodes[tid].no_of_edges + d_graph_nodes[tid].starting); i++){
            int id = d_graph_edges[i];
            if(!d_graph_visited[id]){
              d_cost[id]=d_cost[tid]+1;
              //d_graph_mask[id]=true;
              //d_over[0] = 1;
							d_updating_graph_mask[id]=true;
            }
          }
        }
      }
#pragma omp target teams distribute parallel for num_teams(num_of_teams) thread_limit(MAX_THREADS_PER_BLOCK)
			for (int tid = 0; tid < no_of_nodes; tid++) {
			   if(d_updating_graph_mask[tid]){
			   		d_graph_mask[tid]=true;
			      d_graph_visited[tid]=true;
			      d_over[0]=1;
			      d_updating_graph_mask[tid]=false;
			   }
			}
#pragma omp target update from (d_over[0:1])
    } while (d_over[0]);
  }
}
