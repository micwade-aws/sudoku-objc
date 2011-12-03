/* Michael J Wade
 * mike@iammyownorg.org
 * 
 * main.m
 * Use A* to solve Sudoku Puzzles.
 */

#import <stdio.h>
#import "objc/objc.h"
#import "icpak102.h"
#import "rngs.h"
#import "rvgs.h" 

#import "Individual.h"

#define CHECKPT {printf("Checkpoint: %s, line %d\n", __FILE__,__LINE__);\
		fflush(stdout);}

void usage(){
	printf ("\tSudoku Puzzle Factory\n");
	printf ("Usage: ./a.out <puzzle> <m> <n> <sm> <sn> <maxFit> \n");
	exit(0);
}

int main(int argc, char *argv[]){

	//A* Parameters & Pointers
	id individual, myStar;
	int m,n,sm,sn,mf,genCount,sol;
	int i,x,howMany;
	FILE *fp;
	
	//Check the inputs
	if(argc < 7){
		usage();
	}else{
		if(!(fp   = fopen(argv[1], "r"))){
			printf("Unable to open puzzle file\n");
			exit(0);
		}
		m    = atoi(argv[2]);
		n    = atoi(argv[3]);
		sm   = atoi(argv[4]);
		sn   = atoi(argv[5]);
		mf   = atoi(argv[6]);

		howMany = atoi(argv[7]);
	}
	
	//Create a new individual from a given puzzle file
	individual = [  [Individual new] 
			initialize:fp m:m n:n subm:sm subn:sn maxFit:mf
		     ];
	fclose(fp);

	if(howMany > [individual chromLen])
		return 0;

	for(i=0;i < howMany; i++)
	{
		x = Equilikely(0,(m*m)-1);
		if( [individual alleleAt:x] )
			[individual gene:(x) allele:0];
		else
			i--;
	}

	[individual printChromosoneOn:(stdout)];
	[individual free];

	return 0;
}
