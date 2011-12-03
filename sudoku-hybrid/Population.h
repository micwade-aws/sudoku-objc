/* Michael J Wade
 * mike@iammyownorg.org
 * (c) 2007
 * 
 * Population.h
 * Implementation of a Population of Individuals
 */

#import "objc/objc.h"
#import "objc/Object.h"
#import "icpak102.h"
#import "ordcltn.h"
#import "idarray.h"
#import "rvgs.h"
#import "Individual.h"

@interface Population : Object {
	id population;
	int popSize, nCO, nMut, nGen;
	double maxFit, avgFit, sumFit;
	double pMut, pCO;
}

/********************************************************************* 
 *Essential
 * 	These functions are essential to init,allocate,and delete
 *	a population to and from existence.
 *********************************************************************/
- initPop:(int)size with:anIndividual pMutation:(double)pm pCrossover:(double)pc;
- free;
- at:(int)loc;

/********************************************************************* 
 *Selection Functions
 * 	These functions are interchangeable as they must be passed in
 *	to the evolution function as a selector
 *********************************************************************/
- randomSelect;
- mostFitIndividual;
- binomialSelect;

/********************************************************************* 
 *Genetic Functions
 * 	These functions constitute a collection of functions used in
 *	implementations of the genetic algorithm.
 *********************************************************************/
- crossover:A with:B;
- crossoverWholeRow:A with:B;
- mutate:anIndividual;
- evolve:(SEL)aSelectMethod;
- scorePopulation;
- mutationRate:(double)p;

/********************************************************************* 
 *Parameters 
 * 	Messages to return instance parameters from a population
 *********************************************************************/
- (int) size;
- (int) maxFit;
- (double) avgFit;
- (int) sumFit;
- (double) mutationRate;
- (int) totalMutations;

/********************************************************************* 
 *PrintOns 
 * 	Print individuals, entire populations, statistics, etc.
 *********************************************************************/
- printOn: (IOD)anIOD;
- printBestFitOn: (IOD)anIOD;
- printReportOn:(IOD)anIOD;
- printStatsOn: (IOD)anIOD;

@end
