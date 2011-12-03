// magic.c
// Michael J Wade
// 1-30-06
//
// C implementation of Magic Squares

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void MakeMagic( int *grid, int A, int N ){
	int i,j;

	for(j=0; j<N; j++)
		for(i=0; i<N; i++)
			grid[(N*i)+j] = ((i - (j*A)) % N);
	return;
}
void ShowMagic(int *grid, int N){
	int i,j;

	for(j=0; j<N; j++){
		for(i=0; i<N; i++){
			printf(" (%d,%d) %d ",i,j,grid[(N*i)+j]);
		}
		printf("\n");
	}

	return;
}

int main(int argc, char *argv[]){
	
	int A, B, N, *grid;
	//Get Inputs
	N = atoi(argv[1]);
	A = atoi(argv[2]);
	B = atoi(argv[3]);
	grid = (int *)calloc(N*N, sizeof(int));

	MakeMagic(grid, A, N);

	ShowMagic(grid, N);

	return 0;
}
