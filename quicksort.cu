#include <cuda.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_DEPTH       16
#define INSERTION_SORT  32
#define N 8

void gpu_qsort(int *data, int nitems);

__global__ void gpu_qsort_Kernel(int *arr, int left, int right, int depth){

    cudaStream_t sl,sr;

    int *lptr = arr + left;
    int *rptr = arr + right;
    int pivot = arr[(left+right)/2];

    int lval, rval;
    int nright, nleft;

    while (lptr <= rptr){
        lval = *lptr;
        rval = *rptr;

        // move elements smaller than the pivot value to left subarray
        while (lval < pivot && lptr < arr+right){
            lptr++;
            lval = *lptr;
        }

	// move elements larger than the pivot value to right subarray
        while (rval > pivot && rptr > arr+left){
            rptr--;
            rval = *rptr;
        }

        // swap
        if (lptr <= rptr){
            *lptr = rval;
            *rptr = lval;
            lptr++;
            rptr--;
        }
    }

    nright = rptr - arr;
    nleft  = lptr - arr;

    // Launch a new block to sort the left part.
    if (left < nright){
        cudaStreamCreateWithFlags(&sl, cudaStreamNonBlocking);
        gpu_qsort_Kernel<<< 1, 1, 0, sl >>>(arr, left, nright, depth+1);
        cudaStreamDestroy(sl);
    }

    // Launch a new block to sort the right part.
    if (nleft < right){
        cudaStreamCreateWithFlags(&sr, cudaStreamNonBlocking);
        gpu_qsort_Kernel<<< 1, 1, 0, sr >>>(arr, nleft, right, depth+1);
        cudaStreamDestroy(sr);
    }
}


void gpu_qsort(int *data, int n){
    int* temp;
    int left = 0;
    int right = n-1;
    int block_size = 1;
    int threads_per_block = 10;

    // set the max depth
    cudaDeviceSetLimit(cudaLimitDevRuntimeSyncDepth, MAX_DEPTH);

    cudaMalloc((void**)&temp, n*sizeof(int));

    cudaMemcpy(temp, data, n*sizeof(int), cudaMemcpyHostToDevice);

    gpu_qsort_Kernel <<< block_size, threads_per_block >>> (temp, left, right, 0);

    cudaDeviceSynchronize();

    cudaMemcpy(data, temp, n*sizeof(int), cudaMemcpyDeviceToHost);

    cudaFree(temp);
    // clean up the driver 
    cudaDeviceReset();
}


int main(int argc, char* argv[]){
	int n; //problem size
	FILE* fp;
	FILE* fp_out;
	char outname[100];

	fp = fopen(argv[1], "r");

        fscanf(fp, "%d", &n);

        int* inputarr = (int*)malloc(n*sizeof(int));

        for(int i = 0; i < n; i++){
        	fscanf(fp, "%d", &inputarr[i]);
        }

	gpu_qsort(inputarr, n);

	sprintf(outname, "%d", n);
        strcat(outname, ".txt");
	fp_out = fopen(outname, "w");

	for(int i = 0; i < n; i++) fprintf(fp_out, "%d ", inputarr[i]);

}
