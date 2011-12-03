 
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
	int suggested; //This is the value "suggested" by the SGA :)
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
	int nOpen, nClosed;	//# open, # closed
	int iOpen, iClosed;	//Initial # Open & Closed
	int *totals; 		//Keep track of what is used
}

/********************************************************************* 
 *Essential
 * 	These functions are essential to init,allocate,and delete
 *	an Individual to and from existence.
 *********************************************************************/
- copy;
- free;
- initialize:(IOD)anIOD m:(int)m n:(int)n subm:(int)sm subn:(int)sn maxFit:(int)mf;
- load:(IOD)anIOD m:(int)m n:(int)n subm:(int)sm subn:(int)sn maxFit:(int)mf;

/********************************************************************* 
 * Operators
 * 	Perform modifications to a chromosone
 *********************************************************************/
- randomize;
- gene:(int)loc allele:(int)val; 
- gene:(int)loc allele:(int)val suggest:(int)sVal;
- removeUncertainty:(Gene *)gene below:(double)t;
- correctChromosone;
- cleanChromosome;
- prefaceWith: anIndividual;

/***********************************************************
 * Evaluators
 *	Evalutations of fitness and comparitive operations
 ***********************************************************/
- (signed int) evalVector:(int*)v;
- (int) evalFitness;
- (double) certaintyOf:(Gene *)gene;
- (double) MU;
- (int) vague;
- (BOOL) compareFitness: anIndividual;
- (BOOL) moveRemainsAt:(int)loc;
- (int) mostLikelyAt:(int)loc;

/********************************************************************* 
 *Parameters 
 * 	Messages to return instance parameters from an Individual
 *********************************************************************/
- (double) phenotype;
- (int) fitness;
- fitness:(int)newFitVal;
- (int) maxAllele;
- (int) chromLen;
- (int) rowLen;
- (int) alleleAt:(int)loc;
- (int) statusAt:(int)loc;
- (int) open;
- (int) closed;
- (int) initOpen;
- (int) initClosed;

/********************************************************************* 
 *PrintOns 
 * 	Various ways to display the chromosone or statistics.
 *********************************************************************/
- printOn:(IOD)anIOD;
- printChromosoneOn:(IOD)anIOD;
- printCertainty:(IOD)anIOD;

@end
