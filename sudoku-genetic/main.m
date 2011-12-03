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

#define CHECKPT {printf("Checkpoint: %s, line %d\n", __FILE__,__LINE__);\
		fflush(stdout);}

void usage(){
	printf ("\tGENETIC EVOLTUION\n");
	printf ("Usage: ./a.out <puzzle> <m> <n> <sm> <sn> <maxFit> <pop. size> <pMut> <pCO>\n");
	exit(0);
}

int main(int argc, char *argv[]){

	//SGA Parameters & Pointers
	id individual, pop;
	int size,m,n,sm,sn,mf,mr,genCount;
	int lastMF,mfWatch;
	double pm,pc;
	FILE *fp;

	//TIming Variables
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
	//[individual printOn:(stdout)];
	//[individual printCertainty:stdout];
	
	//Setup for genetic evolution using the new individual as a template
	pop = [Population new];
	[pop initPop:size with:individual pMutation:pm pCrossover:pc];
	[individual free]; //don't need to save this template anymore.	
	
	//[[pop at:0] printChromosoneOn:(stdout)];
	//[[pop at:0] correctChromosone];
	//[[pop at:0] printChromosoneOn:(stdout)];
	//return 1;

	//Evolve a solution
	genCount=0;
	lastMF = mfWatch = 0;
	while( ([pop maxFit] < mf && [[pop at:0] MU] < 0.9) /*&& genCount < 16000*/)
	{	
		if(mfWatch > 20){
			[pop mutationRate:([pop mutationRate]+0.1)];
			mfWatch = 0;
			if([pop mutationRate] > 0.4){
				//Stop processing and print the current status		
				gettimeofday(&end_time,NULL); 
				delta_time = (double)(end_time.tv_sec - start_time.tv_sec); 
				delta_time += (double)(end_time.tv_usec - start_time.tv_usec)/1.0e+6; 
				[pop printStatsOn:(stdout)];
				printf(" %lf\n",delta_time); 
				return 0;
				[pop mutationRate:0.1];
			}
		}


		//[pop evolve:@selector(randomSelect)];
		//[pop evolve:@selector(binomialSelect)];
		[pop evolve:@selector(mostFitIndividual)];
		[[pop at:0] evalFitness];
		//[pop evolve:@selector(topTwo)];
		genCount++;
		//if(!(genCount % 1000))
			//printf("Generation:%d\tAvg.Fit:%3.2f\tMax.Fit:%d\tSum.Fit:%d\tpMut:%f\n",genCount,[pop avgFit],[pop maxFit],[pop sumFit],[pop mutationRate]);
		//if(!(genCount % 100))
		//	printf("#Generation Avg.Fit Max.Fit Sum.Fit pMut\n");
		//[pop printStatsOn:(stdout)];

		if(genCount == 1)
			lastMF = [pop maxFit];
		else{
			if(lastMF == [pop maxFit])
				mfWatch++;
		}
		lastMF = [pop maxFit];
		/*	
		if(genCount > 1000){
			
			//Stop processing and print the current status		
			gettimeofday(&end_time,NULL); 
			delta_time = (double)(end_time.tv_sec - start_time.tv_sec); 
			delta_time += (double)(end_time.tv_usec - start_time.tv_usec)/1.0e+6; 
			[pop printStatsOn:(stdout)];
			printf(" %lf\n",delta_time); 

			//return 0;
		}*/
	}

	//Show the results
	//[pop printReportOn:(stdout)];
	//printf("#Generation Avg.Fit Max.Fit Sum.Fit pMut\n");
	gettimeofday(&end_time,NULL); 
	delta_time = (double)(end_time.tv_sec - start_time.tv_sec); 
	delta_time += (double)(end_time.tv_usec - start_time.tv_usec)/1.0e+6; 
	[pop printStatsOn:(stdout)];
	printf(" %lf\n",delta_time); 


	[pop free];

	return 0;
}
