 
/* Individual.h
 * Individual container class
 *
 * Michael J Wade 
 * mike@iammyownorg.org 
 * Copyright (c) 2007
 * 
 */

#import <stdio.h>
#import "objc/objc.h"
#import "objc/Object.h"
#import "icpak102.h"


#define OPEN   1
#define CLOSED 0
typedef struct gene {
	int allele;
	int status;
	int xLoc, yLoc, Loc;			
	int *possibles;
	double score; //Astar specific score value
} Gene;

@interface Individual : Object {
@public
	Gene *chromosone;	//Dynamically allocate the chromosone. 
	double phenotype; 	//Measures interaction w/ environment.
	int fitness;		//Value given a fitness function
	int parent1, parent2; 	//Location of parents in population
	int xSite;		//Place for crossover to occur??
	int ni,nj;		//The chromosone forms an mXn matrix
	int sub_ni,sub_nj;	//Sub regions within the chromosone
	int chromLen;		//Length of the chromosone
	int maxAllele;		//Maximum value in a gene
	int maxFit;		//Maximum fitness of a chromosone
	int nOpen, nClosed;	//# open, # closed.  
	int *totals; 		//Keep track of what is used
}

- copy;
- free;
- initialize:(IOD)anIOD m:(int)m n:(int)n subm:(int)sm subn:(int)sn maxFit:(int)mf;
- load:(IOD)anIOD m:(int)m n:(int)n subm:(int)sm subn:(int)sn maxFit:(int)mf;
- gene:(int)loc allele:(int)val; 
- randomize;

/***********************************************************
 * Evaluators
 *	Evalutations of fitness and comparitive operations
 ***********************************************************/
- (signed int) evalVector:(int*)v;
- (int) evalFitness;
- (double) certaintyOf:(Gene *)gene;
- removeUncertainty:(Gene *)gene below:(double)t;
- (double) MU;
- (int) vague;
- (BOOL) compareFitness: anIndividual;

- (BOOL) moveRemainsAt:(int)loc;
- (int) mostLikelyAt:(int)loc;

- (double) phenotype;
- (int) fitness;
- fitness:(int)newFitVal;
- (int) maxAllele;
- (int) chromLen;
- (int) alleleAt:(int)loc;
- (int) statusAt:(int)loc;
- (int) open;
- (int) closed;

- printOn:(IOD)anIOD;
- printChromosoneOn:(IOD)anIOD;
- printCertainty:(IOD)anIOD;

@end
