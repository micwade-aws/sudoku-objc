/* Individual.h
 * Individual class implementation file.
 *
 * Michael J Wade 
 * mike@iammyownorg.org 
 * Copyright (c) 2010
 * 
 */
#pragma once
#include "Gene.h"

class Individual
{
public:
		
	static const int OPEN = 1;
	static const int CLOSED = 0;
	
	Gene *chromosone;		//Dynamically allocate the chromosone. 
	double m_phenotype; 		//Measures interaction w/ environment.
	int m_fitness;			//Value given a fitness function
	int parent1, parent2; 	//Location of parents in population
	int xSite;				//Place for crossover to occur??
	int ni,nj;				//The chromosone forms an mXn matrix
	int sub_ni,sub_nj;		//Sub regions within the chromosone
	int m_chromLen;			//Length of the chromosone
	int m_maxAllele;			//Maximum value in a gene
	int m_maxFit;				//Maximum fitness of a chromosone
	int nOpen, nClosed;		//# open, # closed
	int iOpen, iClosed;		//Initial # Open & Closed
	int *totals; 			//Keep track of what is used
		
	/********************************************************************* 
	 * Essential
	 * 	These functions are essential to init,allocate,and delete
	 *	an Individual to and from existence.
	 *********************************************************************/
	Individual(void);	
	Individual(const Individual &x);
	~Individual(void);
	Individual(void *pFile, int m, int n, int subm, int subn, int mf);
	void load(void *pFile, int m, int n, int subm, int subn, int mf);
	
	/********************************************************************* 
	 * Operators
	 * 	Perform modifications to a chromosone
	 *********************************************************************/
	void randomize();
	void setAllele(int loc, int val); 
	void setAllele(int loc, int val, int suggestedVal); 
	void removeUncertainty(Gene *pGene, double belowThreshold);
	void correctChromosone();
	void cleanChromosome();
	void prefaceWith(Individual &anIndividual);

	/***********************************************************
	 * Evaluators
	 *	Evalutations of fitness and comparitive operations
	 ***********************************************************/
	signed int evalVector(int *pV);
	int evalFitness();
	double certaintyOf(Gene *pGene);
	double MU();
	int vague();
	bool compareFitness(Individual &anIndividual);
	bool moveRemainsAt(int loc);
	int mostLikelyAt(int loc);

	/********************************************************************* 
	 * Parameters 
	 * 	Messages to return instance parameters from an Individual
	 *********************************************************************/
	double const& phenotype() const;         // Getter   
	void phenotype(double const& newVal);    // Setter
	int const& fitness() const;         // Getter   
	int fitness(int const& newVal);    // Setter
	int const& maxAllele() const;         // Getter   
	int maxAllele(int const& newVal);    // Setter
	int const& chromLen() const;         // Getter   
	int chromLen(int const& newVal);    // Setter
	int const& rowLen() const;         // Getter   
	int rowLen(int const& newVal);    // Setter
	int alleleAt(int loc);
	int statusAt(int loc);
	int open();
	int closed();
	int initOpen();
	int initClosed();

	/********************************************************************* 
	 * PrintOns 
	 * 	Various ways to display the chromosone or statistics.
	 *********************************************************************/
	/*- printOn:(IOD)anIOD;
	- printChromosoneOn:(IOD)anIOD;
	- printCertainty:(IOD)anIOD;*/
};
