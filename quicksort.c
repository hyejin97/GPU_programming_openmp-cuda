#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "omp.h"

#define MAX_DEPTH 16
#define NUM_THREADS 8

void omp_qsort(int *arr, int nitems);

void qsort_helper(int *arr, int left, int right, int depth){

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

    // Launch a new block to sort the left part.
    if (left < nright){
        #pragma omp task 	
	{
		qsort_helper(arr, left, nright, depth+1);
	}
    }

    // Launch a new block to sort the right part.
    if (nleft < right){
	#pragma omp task  
	{
        	qsort_helper(arr, nleft, right, depth+1);
	}
    }
}


void omp_qsort(int *arr, int n){

	omp_set_num_threads(NUM_THREADS);

	#pragma omp target data map(tofrom: arr) map(to: n)
	#pragma omp parallel
	{
		#pragma omp single nowait  
		{
			qsort_helper(arr, 0, n-1, 0);
		}
	}

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
