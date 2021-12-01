#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]){
	//gets n(number of elements) as an agument when running the program
	int n = atoi(argv[1]);
	FILE* fp_out;
	char outname[100];

	sprintf(outname, "%d", n);
        strcat(outname, "in.txt");

        fp_out = fopen(outname, "w");
	
	fprintf(fp_out, "%d\n", n);
	for(int i = 0; i < n; i++){
		int r = rand() % n;
		fprintf(fp_out, "%d ", r);	
	}
}
