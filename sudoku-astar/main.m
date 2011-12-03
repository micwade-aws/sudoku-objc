/* Michael J Wade
 * mike@iammyownorg.org
 * 
 * main.m
 * Use A* to solve Sudoku Puzzles.
 */

#import <stdio.h>
#import "objc/objc.h"
#import "icpak102.h"

#import "Astar.h"
#import "Individual.h"

#define CHECKPT {printf("Checkpoint: %s, line %d\n", __FILE__,__LINE__);\
		fflush(stdout);}

void usage(){
	printf ("\tA* Sudoku Solver\n");
	printf ("Usage: ./a.out <puzzle> <m> <n> <sm> <sn> <maxFit> \n");
	exit(0);
}

int main(int argc, char *argv[]){

	//A* Parameters & Pointers
	id individual, myStar;
	int m,n,sm,sn,mf,genCount,sol;
	FILE *fp;
	struct timeval start_time; 
	struct timeval end_time; 
	double delta_time; 
	
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
	}
	
	//Ready To Begin
	gettimeofday(&start_time,NULL);
	
	//Create a new individual from a given puzzle file
	individual = [  [Individual new] 
			initialize:fp m:m n:n subm:sm subn:sn maxFit:mf
		     ];
	fclose(fp);
	//printf("fitness: %d\n", [individual evalFitness]);

	//Initialize A* Solution Search
	myStar = [Astar new];
	[myStar initialize:individual];
	
	//[individual printChromosoneOn:(stdout)];
	//return 0;
	//[individual printCertainty:(stdout)];
	//If given, tell A* to record the run statistics on in argv[7]
	if(argc == 8){
		if(!(fp   = fopen(argv[7], "w"))){
			printf("Unable to open stats file!\n");
			exit(0);
		}
		[myStar recordOn:fp];
	}

	//Find the solution, if it exists
	sol = [myStar solve];

	//Stop the timer for the program.
	gettimeofday(&end_time,NULL); 
	delta_time = (double)(end_time.tv_sec - start_time.tv_sec); 
	delta_time += (double)(end_time.tv_usec - start_time.tv_usec)/1.0e+6; 
 
	if(argc == 8)
		fclose(fp);
	
	if(!sol){
		//printf("UNSOLVEABLE!\n");	
		[myStar printStatsOn:(stdout)];
		printf(" %lf\n",delta_time);
	}else{
		//Print the unchanged chromosone to show the initial puzzle
		[myStar printStatsOn:(stdout)];
		printf(" %lf\n",delta_time);
		[individual printChromosoneOn:(stdout)];
		//[individual printCertainty:(stdout)];
		//[individual vague];
		//[individual printOn:(stdout)];
	}
	

	return 0;
}
