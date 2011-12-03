/* Michael J Wade
 * mike@iammyownorg.org
 * 
 * Population.m
 * Implementation of a Population of Individuals.
 */

#import "objc/objc.h"
#import "objc/Object.h"
#import "icpak102.h"
#import "ordcltn.h"
#import "Individual.h"
#import "Population.h"

#define CHECKPT {printf("Checkpoint: %s, line %d\n", __FILE__,__LINE__);\
		fflush(stdout);}

@implementation Population

/********************************************************************* 
 *Essential
 * 	These functions are essential to init,allocate,and delete
 *	a population to and from existence.
 *********************************************************************/

/***********************************************************
 * PRE: none.
 * POST: The population is populated and parameters are setup.
 * NOTE: Each memeber of the new population gets their 
 *	 chromosone randomized!
 ***********************************************************/
- initPop:(int)size with:anIndividual pMutation:(double)pm pCrossover:(double)pc {
	id A;
	int i;
	
	//Setup the Population.
	population = [ IdArray new ];
	[population capacity:size ];
	maxFit 	= 0;
	avgFit  = 0.;
	sumFit  = 0;
	pMut	= pm;
	pCO	= pc;
	nCO	= 0;
	nMut	= 0;
	nGen	= 0;
	
	//Create n new individuals with random chromosones
	for(i = 0; i<size; i++){
		[ population add:[anIndividual copy] ];
		[ [population at:i] randomize ]; 
	}
	
	popSize = [ population size ];

	return self;
}

/***********************************************************
 * PRE: none.
 * POST: Members of the population are freed.  
 ***********************************************************/
- free {
	[population freeContents];
	[super free];
	return nil;	
}

/***********************************************************
 * PRE: none.
 * POST: Returns pointer to a particular member of the population
 ***********************************************************/
- at:(int)loc {
	return [population at:loc];
}

/********************************************************************* 
 *Selection Functions
 * 	These functions are interchangeable as they must be passed in
 *	to the evolution function as a selector
 *********************************************************************/

/***********************************************************
 * PRE: none.
 * POST: random individual is returned.  
 * TODO: Insert the Boltzmann factor heuristic from Duda (p.376)
 ***********************************************************/
- randomSelect {
	return [population at:(int)(Equilikely(0,(popSize-1)))];
}

/***********************************************************
 * PRE: none.
 * POST: random individual is returned.
 * NOTE: This selection is biased towards the top 10%.  
 ***********************************************************/
- binomialSelect {
	return [population at:(int)(Binomial((popSize-1),0.1))];
}


/***********************************************************
 * PRE: none.
 * POST: Returns individual deemed "most fit"
 * NOTE: Avoids re-selection by setting fitness to 0.
 * TODO: Find a way around setting the fitness to 0.
 ***********************************************************/
- mostFitIndividual {
	int i;
	
	for(i = 0; i < popSize; i++)
		if( [[population at:i]	fitness] == maxFit ){
			[[population at:i] fitness:0]; //avoid reselect
			return [population at:i];
		}

	//If none of the individuals match the maxFit variable
	//Then the population has not been evaluated.
	//Evaluate the population, then return the new maxFit.
	[self scorePopulation];

	return [self mostFitIndividual];
}


/********************************************************************* 
 *Genetic Functions
 * 	These functions constitute a collection of functions used in
 *	implementations of the genetic algorithm.
 *********************************************************************/


/********************************************************************* 
 * PRE: none.
 * POST: New individual is formed by crossover of A and B.
 * NOTE: Crossover will choose a random location on A's 
 *	 chromosone. Then copy everything before that location 
 *	 into C. B will copy everything after that location into C.
 *********************************************************************/
- crossover:A with:B {
	id C;
	int splicePt, maxLength, i;
	
	//Create a new Individual
	C = [A copy];	

	//Point in the chromosone to crossover at
	maxLength = [A chromLen];
	splicePt = Equilikely(1,(maxLength-1)); //Note: can't be 0
	//printf("splice pt. = %d\n",splicePt);

	//Copy everything b4 splicePt from A to C
	for(i = 0; i < splicePt; i++){
		[C gene:i allele:[A alleleAt:i] ];
	}
	
	//Copy everything after splicePt from B to C
	for(i = splicePt; i < maxLength; i++)
		[C gene:i allele: [B alleleAt:i] ];

	return C;
}

/********************************************************************* 
 * PRE: none.
 * POST: New individual is formed by crossover of A and B.
 * NOTE: WholeRow will take every other row from A and B to make C.
 *********************************************************************/
- crossoverWholeRow:A with:B {
	id C;
	int maxLength, i,j;
	
	//Create a new Individual
	C = [B copy];	

	//Copy every other row from A into C
	for(j = 0; j < [C rowLen]; j++)
	{
		if(j%2)
			for(i = 0; i < [C rowLen]; i++)
				[C gene:([C rowLen]*i+j) allele:[A alleleAt:([C rowLen]*i+j)] ];
	}
	
	return C;
}

/********************************************************************* 
 * PRE: none.
 * POST: An Individual's chromosone has been mutated.
 * NOTE: This crossover will use the certainty of gene alleles to 
 *	 determine dominance of the gene.  The hope is that certainty
 *	 will trump out invalid gene values without addt'l computation.
 * TODO: Insert the certainty value into the Gene structure.
 *********************************************************************/
- vCrossover:A with: B {
	int i,chromLen;
	double cA,cB;	
	id C;
	
	//Create a new Individual
	C = [A copy];	
	chromLen = [C chromLen];
	
	for(i=0;i<chromLen;i++){
		cA = [A certaintyOf:&((Individual*)A)->chromosone[i]];
		cB = [B certaintyOf:&((Individual*)B)->chromosone[i]];
		if(cB > cA)
			[C gene:i allele: [B alleleAt:i] ];
		else
			[C gene:i allele: [A alleleAt:i] ];
	}
	return C;
}

/********************************************************************* 
 * PRE: none.
 * POST: An Individual's chromosone has been mutated.
 * NOTE: Mutation is based on P(mutation) 
 * TODO: Check the inputs before processing (especially pMut).
 *********************************************************************/
- mutate:anIndividual {

	id mutation;
	int i, maxLength, maxAllele, oldAllele, newAllele;

	//Grab a few control variables from the individual
	maxLength = [anIndividual chromLen];
	maxAllele = [anIndividual maxAllele];

	//Create a new individual to represent the mutated individual
	mutation  = [anIndividual copy];

	for(i = 0; i < maxLength; i++){
		//Mutate any open gene given some probability
		if( Bernoulli(pMut)  && [mutation statusAt:i] == OPEN){
			
			//Find a new pseudo-random allele value
			oldAllele = newAllele = [mutation alleleAt:i ];
			while( newAllele == oldAllele )
				newAllele = Equilikely(1,maxAllele);

			//Set the allele value for the gene
			[mutation gene:i allele:newAllele ];

			//Record the total mutations
			nMut++;
		}
	}
	
	return mutation;
}

/********************************************************************* 
 * PRE: none.
 * POST: The population has evolved to the next generation.
 * NOTE: The user is allowed to specify the method of selection!
 *********************************************************************/
- evolve:(SEL)aSelectMethod {

	id individualA, individualB, individualC;
	int i;
	//Be certain that the seletor is valid.
   	SEL selectMethod = ISSELECTOR(aSelectMethod) ? aSelectMethod :sel_get_uid((char *)aSelectMethod);

    if (selectMethod){

	//Create room to store the new population
	[population capacity:popSize*2];

	//Append a new generation to the population
	for(i = 0; i < popSize; i++){
		//Select two individuals
		individualA = [self perform:selectMethod];
		individualB = [self perform:selectMethod];
		//individualA = [population at:0];
		//individualB = [population at:1];
		
		
		//Perform crossover with probability pCO
		//Otherwise, mutate both and reinsert into population.
		if( Bernoulli(pCO) ){
			nCO++;//track crossovers
			//individualC = [self crossover:individualA with:individualB];
			individualC = [self crossoverWholeRow:individualA with:individualB];
			//individualC = [self vCrossover:individualA with:individualB];
			//nMut++;//track mutations
			//individualC = [self mutate:individualC];
		}else{
			nMut++;//track mutations
			individualC = [self mutate:individualA];
		}

		//Insert the new individual into the population
		[population add:individualC];

		//Remove the errors from the new individual
		[individualC correctChromosone];
//CHECKPT
	}	

	//Remove the old population
	//The removed member must also be freed!
	for(i = 0; i < popSize; i++)
		[[population remove:[population at:i] ] free];

	//Free the empty spots in the population
	[population packContents];

	//Score the population 
	[self scorePopulation];

	//Sort the population (see Individual's compare:)
	[population sortBy:@selector(compareFitness:)];

#ifdef CHECKSORT
	for(i=0;i<popSize;i++)
		printf("%d=%d\n",i,[ [population at:i] fitness]);
#endif
	//Increment the generation counter
	nGen++; 
   }
	return self;
}

/********************************************************************* 
 * PRE: There must be members in the population.
 * POST: Each member has been evaluated for fitness and assigned their
 *	 new fitness value.
 * NOTE: This function does not sort.
 *********************************************************************/
- scorePopulation {
	int i, f;

	//Force each individual to update their fitness value
	//Also, record the maximum fitness value found.
	f = maxFit = sumFit = 0;
	for(i = 0; i < popSize; i++){
		f = [ [population at:i] evalFitness];
		sumFit += f;
		if(f > maxFit) 
			maxFit = f;
	}

	avgFit = ((double)sumFit) / ((double)popSize);

	return self;
}

/********************************************************************* 
 * PRE: none.
 * POST: The probability of mutation is updated to 'p'
 *********************************************************************/
- mutationRate:(double)p {
	pMut = p;
	return self;
}


/********************************************************************* 
 *Parameters 
 * 	Messages to return instance parameters from a population
 *********************************************************************/
- (int) size		{return [population size]; }
- (int) maxFit		{return maxFit; }
- (double) avgFit	{return avgFit; }
- (int) sumFit		{return sumFit; }
- (int) totalMutations 	{return nMut;	}
- (double) mutationRate {return pMut;	}


/********************************************************************* 
 *PrintOns 
 * 	Print individuals, entire populations, statistics, etc.
 *********************************************************************/

/********************************************************************* 
 * PRE: none.
 * POST: Each member of the population is printed.
 *********************************************************************/
- printOn: (IOD) anIOD {
	int i;

	for(i = 0; i<popSize; i++)
		[[population at:i] printOn:anIOD];

	return self;
}

/********************************************************************* 
 * PRE: none.
 * POST: Prints individual with the highest fitness score.
 *********************************************************************/
- printBestFitOn:(IOD)anIOD {
	id A;

	A = [self mostFitIndividual];
	[A printChromosoneOn:anIOD];

	return self;
}

/********************************************************************* 
 * PRE: none.
 * POST: Prints a human readable report of the generation.
 * TODO: Increase the usefulness of this statistical report
 *********************************************************************/
- printReportOn: (IOD) anIOD{
	fprintf(anIOD, "--1-----1-0------0-----------GENETIC ALGORITHM STATISTICS---------1-------0-----------1-\n");
	fprintf(anIOD, "General Runtime Results:\n");
	fprintf(anIOD, "Variable\t|\tValue\n");
	fprintf(anIOD, "----------------|----------\n");
	fprintf(anIOD, "Pop. Size\t|\t%d\n",popSize);
	fprintf(anIOD, "Generations\t|\t%d\n",nGen);
	fprintf(anIOD, "Crossovers\t|\t%d\n",nCO);
	fprintf(anIOD, "P(Crossover)\t|\t%f\n",pCO);
	fprintf(anIOD, "Mutations\t|\t%d\n",nMut);
	fprintf(anIOD, "P(Mutations)\t|\t%f\n",pMut);
	fprintf(anIOD, "----------------|----------\n");
	fprintf(anIOD, "Maximum Fit\t|\t%d\n",maxFit);
	fprintf(anIOD, "Average Fit\t|\t%f\n",avgFit);
	fprintf(anIOD, "Sum Fit\t|\t%d\n",sumFit);
	fprintf(anIOD, "--1-----1-0------0-----------The Most Fit Individual--------------1-------0-----------1-\n");
	[self printBestFitOn:anIOD];
	fprintf(anIOD, "--1-----1-0------0----------0------------1------------01----------1-------0-----------1-\n");
	return self;
}

/********************************************************************* 
 * PRE: none.
 * POST: Prints general stats to anIOD
 *********************************************************************/
- printStatsOn: (IOD)anIOD {
	/*
         * init. open | init closed | init C | # checks | C | open | closed | bad guesses
         */
        //fprintf(anIOD,"%d %d %f %d %f %d %d %d %d\n", initOpen, initClosed, initC, checkCount, [puzzle MU], [puzzle open], [puzzle closed], badGuesses, [puzzle evalFitness]);

	fprintf(anIOD,"%d %d %1.3f %d %3.2f %d %d %f",[[population at:0] initOpen],[[population at:0] initClosed],[[population at:0] MU],nGen,[self avgFit],[self maxFit],[self sumFit],[self mutationRate]);

	return self;
}
@end
