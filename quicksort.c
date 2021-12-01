#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "omp.h"

#define MAX_DEPTH 16
#define NUM_THREADS 8

void omp_qsort(int *arr, int nitems);

void qsort_helper(int *arr, int left, int right, int depth, float* seconds){
    clock_t start = clock();

    int *lptr = arr + left;
    int *rptr = arr + right;
    int pivot = arr[(left+right)/2];

    int lval;
    int rval;

    while (lptr <= rptr){
        lval = *lptr;
        rval = *rptr;

        while (lval < pivot && lptr < arr + right){
            lptr++;
            lval = *lptr;
        }

        while (rval > pivot && rptr > arr + left){
            rptr--;
            rval = *rptr;
        }

        if (lptr <= rptr){
            *lptr = rval;
            *rptr = lval;
            lptr++;
            rptr--;
        }
    }

    int nright = rptr - arr;
    int nleft  = lptr - arr;
	
    clock_t end = clock();
    *seconds += (float)(end - start) / CLOCKS_PER_SEC;

    // Launch a new block to sort the left part.
    if (left < nright){
        #pragma omp task 	
	{
		qsort_helper(arr, left, nright, depth+1, seconds);
	}
    }

    // Launch a new block to sort the right part.
    if (nleft < right){
	#pragma omp task  
	{
        	qsort_helper(arr, nleft, right, depth+1, seconds);
	}
    }
}


void omp_qsort(int *arr, int n){
	float* seconds; //calculate time consumed for operations only 
	float total_compute_time = 0.0;

	omp_set_num_threads(NUM_THREADS);
	
	#pragma omp target data map(tofrom: arr) map(to: n)
	#pragma omp parallel
	{
		#pragma omp single nowait  
		{
			seconds = &total_compute_time; 
			qsort_helper(arr, 0, n-1, 0, seconds);
		}
	}

	printf("total computation time: %f\n", *seconds);
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

        omp_qsort(inputarr, n);

        sprintf(outname, "%d", n);
        strcat(outname, ".txt");

        fp_out = fopen(outname, "w");

        for(int i = 0; i < n; i++) fprintf(fp_out, "%d ", inputarr[i]);


}
