/* Michael J Wade
 * mike@iammyownorg.org
 * 
 * main.m
 * Use GA to solve Sudoku Puzzles.
 */

#import <stdio.h>
#import <time.h>
#import "objc/objc.h"
#import "icpak102.h"

#import "Individual.h"
#import "Population.h"
#import "Astar.h"

#define CHECKPT {printf("Checkpoint: %s, line %d\n", __FILE__,__LINE__);\
		fflush(stdout);}

void usage(){
	printf ("\tHYBRID SUDOKU SOLVER\n");
	printf ("Usage: ./a.out <puzzle> <m> <n> <sm> <sn> <maxFit> <pop. size> <pMut> <pCO>\n");
	exit(0);
}

int main(int argc, char *argv[]){

	//SGA Parameters & Pointers
	id individual, pop, myStar, mostFit;
	int size,m,n,sm,sn,mf,mr,genCount;
	int lastMF,mfWatch;
	int sol;
	double pm,pc;
	FILE *fp;

	//Timing Variables
	struct timeval start_time; 
	struct timeval end_time; 
	double delta_time;

	//Check the inputs
	if(argc < 10){
		usage();
	}else{
		fp   = fopen(argv[1], "r");
		m    = atoi(argv[2]);
		n    = atoi(argv[3]);
		sm   = atoi(argv[4]);
		sn   = atoi(argv[5]);
		mf   = atoi(argv[6]);
		size = atoi(argv[7]);
		pm   = atof(argv[8]);
		pc   = atof(argv[9]);
	}

	//Ready To Begin
	gettimeofday(&start_time,NULL);

	//Create a new individual from a given puzzle file
	individual = [  [Individual new] 
			initialize:fp m:m n:n subm:sm subn:sn maxFit:mf
		     ];
	fclose(fp);
	
	//Print the unchanged chromosone to show the initial puzzle
	//[individual printChromosoneOn:(stdout)];
	
	//Setup for genetic evolution using the new individual as a template
	pop = [Population new];
	[pop initPop:size with:individual pMutation:pm pCrossover:pc];
	
	//Evolve a solution
	genCount=0;
	lastMF = mfWatch = 0;
	//while( ([pop maxFit] < mf || [[pop at:0] MU] < 0.7) /*&& genCount < 16000*/){
	while( ([pop maxFit] < mf && [[pop at:0] MU] < 0.65) /*&& genCount < 16000*/){
		
		if(mfWatch > 20){
			[pop mutationRate:([pop mutationRate]+0.1)];
			mfWatch = 0;
			if([pop mutationRate] > 0.4){
				//Stop processing and print the current status		
				//[pop printStatsOn:(stdout)];
				//printf(" %lf\n",delta_time); 
				//return 0;
				break;
				[pop mutationRate:0.1];
			}
		}


		[pop evolve:@selector(randomSelect)];
		//[pop evolve:@selector(binomialSelect)];
		//[pop evolve:@selector(mostFitIndividual)];
		
		[[pop at:0] evalFitness];
		genCount++;
		
		if(genCount == 1)
			lastMF = [pop maxFit];
		else{
			if(lastMF == [pop maxFit])
				mfWatch++;
		}
		lastMF = [pop maxFit];
	}

	//Show the results
	//[pop printReportOn:(stdout)];
	//printf("#Generation Avg.Fit Max.Fit Sum.Fit pMut\n");
	
	//Print the GA Statistics
	[pop printStatsOn:(stdout)];
	printf(" "); //space before A* Stats 

	//Initialize A* Solution Search
	mostFit = [pop at:0];
	[mostFit cleanChromosome];
	
	//What did the GA create?
	//printf("\nThe SGA generated the following:\n");
	//[mostFit printChromosoneOn:(stdout)];

	//This will restore the chromosome back to its original 
	//for, but it will be "prefaced" with the values from SGA
	[individual prefaceWith: mostFit];

	myStar = [Astar new];
	[myStar initWith: individual];
	
	//Find the solution, if it exists
	sol = [myStar solve];
	
	if(!sol){
		printf("UNSOLVEABLE!\n");	
		[myStar printStatsOn:(stdout)];
	}else{
		//What did A* Search create?
		//printf("\nA* Search generated the following:\n");
		[individual printChromosoneOn:(stdout)];

		//Print the unchanged chromosone to show the initial puzzle
		[myStar printStatsOn:(stdout)];
	}
	
	gettimeofday(&end_time,NULL); 
	delta_time = (double)(end_time.tv_sec - start_time.tv_sec); 
	delta_time += (double)(end_time.tv_usec - start_time.tv_usec)/1.0e+6; 
	
	printf(" %lf\n",delta_time);

	[pop free];


	return 1;
}
