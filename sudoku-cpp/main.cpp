/* Michael J Wade
 * mike@iammyownorg.org
 * 
 * main.cpp
 * Use GA to solve Sudoku Puzzles.
 */
#include <iostream>
#include <stdio.h>
#include <time.h>
#include "Puzzle.h"
#include "Individual.h"
#include "Population.h"
#include "Astar.h"
//#include "Display.h"

#define CHECKPT {printf("Checkpoint: %s, line %d\n", __FILE__,__LINE__);\
		fflush(stdout);}

void usage(){
	printf ("\tHYBRID SUDOKU SOLVER\n");
	printf ("Usage: ./a.out <puzzle> <m> <n> <sm> <sn> <maxFit> <pop. size> <pMut> <pCO>\n");
	exit(0);
}

int main( int argc, char* argv[]){

#if 0
	//ASCII strings get converted to puzzles
	char* puzzleString =   ".47258369258369471369471582471582693582693714693714825714825936825936147936147258";
	Puzzle* p = new Puzzle(puzzleString,81);

	//Solve the puzzle
	*if(!p.Solve()){
		printf("Unsolveable.\n");
		return 1;
	}*/

	//Print the solved puzzle
	std::cout << p;
#endif

	//SGA Parameters & Pointers
	Individual myIndividual;
	Population myPop;
	Astar* pAStar;
	Individual* pMostFit;

	int size,m,n,sm,sn,mf,mr,genCount;
	int lastMF,mfWatch;
	int sol;
	double pm,pc;
	FILE *fp;

	//Timing Variables
	/*struct timeval start_time; 
	struct timeval end_time; 
	double delta_time;*/

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
	//gettimeofday(&start_time,NULL);

	//Create a new individual from a given puzzle file
	myIndividual = Individual(fp,m,n,sm,sn,mf);
	fclose(fp);
	
	//Print the unchanged chromosone to show the initial puzzle
	//[individual printChromosoneOn:(stdout)];
	
	//Setup for genetic evolution using the new individual as a template
	myPop = Population(size, myIndividual, pm, pc);
	
	
	//Evolve a solution
	genCount=0;
	lastMF = mfWatch = 0;
	//while( ([pop maxFit] < mf || [[pop at:0] MU] < 0.7) /*&& genCount < 16000*/){
	while( myPop.maxFit() < mf && myPop.at(0)->MU() < 0.65) /*&& genCount < 16000*/{
		
		if(mfWatch > 20){
			myPop.mutationRate(myPop.mutationRate()+0.1);
			mfWatch = 0;
			if(myPop.mutationRate() > 0.4){
				//Stop processing and print the current status		
				//[pop printStatsOn:(stdout)];
				//printf(" %lf\n",delta_time); 
				//return 0;
				break;
				myPop.mutationRate(0.1);
			}
		}

		myPop.evolve(&Population::randomSelect);
		//[pop evolve:@selector(randomSelect)];
		//[pop evolve:@selector(binomialSelect)];
		//[pop evolve:@selector(mostFitIndividual)];
		
		//[[pop at:0] evalFitness];
		myPop.at(0)->evalFitness();
		genCount++;
		
		if(genCount == 1)
			lastMF = myPop.maxFit();
		else{
			if(lastMF == myPop.maxFit())
				mfWatch++;
		}
		lastMF = myPop.maxFit();
	}

	//Show the results
	//[pop printReportOn:(stdout)];
	//printf("#Generation Avg.Fit Max.Fit Sum.Fit pMut\n");
	
	//Print the GA Statistics
	//[pop printStatsOn:(stdout)];
	printf(" "); //space before A* Stats 

	//Initialize A* Solution Search
	pMostFit = myPop.at(0);
	pMostFit->cleanChromosome();
	
	//What did the GA create?
	//printf("\nThe SGA generated the following:\n");
	//[mostFit printChromosoneOn:(stdout)];

	//This will restore the chromosome back to its original 
	//for, but it will be "prefaced" with the values from SGA
	myIndividual.prefaceWith(*pMostFit); ///i don't like this... need to pass pointer
	

	pAStar = new Astar(myIndividual);	
	
	//Find the solution, if it exists
	sol = pAStar->solve();
	
	if(!sol){
		printf("UNSOLVEABLE!\n");	
		//[myStar printStatsOn:(stdout)];
	}else{
		//What did A* Search create?
		//printf("\nA* Search generated the following:\n");
		//[individual printChromosoneOn:(stdout)];

		//Print the unchanged chromosone to show the initial puzzle
		//[myStar printStatsOn:(stdout)];
	}
	
	/*
	gettimeofday(&end_time,NULL); 
	delta_time = (double)(end_time.tv_sec - start_time.tv_sec); 
	delta_time += (double)(end_time.tv_usec - start_time.tv_usec)/1.0e+6; 
	
	printf(" %lf\n",delta_time);
	*/

	//delete everything
	delete pAStar;
	delete pMostFit;

	return 1;
}
