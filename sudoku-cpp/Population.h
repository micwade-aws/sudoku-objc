/* Population.h
 * Implementation of a Population of Individuals
 *
 * Michael J Wade 
 * mike@iammyownorg.org 
 * Copyright (c) 2011
 * 
 */
#pragma once
#include "rvgs.h"
#include "Individual.h"

class Population
{
public:
	int m_popSize, m_nCO, m_nMut, m_nGen;
	double m_maxFit, m_avgFit, m_sumFit;
	double m_pMut, m_pCO;

	///http://www.newty.de/fpt/fpt.html#passPtr
	void* mp_selectFunc(void); // function pointer??
	
	/********************************************************************* 
	 *Essential
	 * 	These functions are essential to init,allocate,and delete
	 *	a population to and from existence.
	 *********************************************************************/
	Population();
	Population(int size, Individual &anIndividual, double probMut /*pm*/, double probCO /*pc*/);
	~Population();
	Individual* at(int loc);  /// This needs a new approach in cpp...

	/********************************************************************* 
	 *Selection Functions
	 * 	These functions are interchangeable as they must be passed in
	 *	to the evolution function as a selector
	 *********************************************************************/
	void randomSelect(void);
	void mostFitIndividual(void);
	void binomialSelect(void);    /// How do I pass functions in cpp?  Function Ptrs?
	/// Selection function pointer in C++
	void (Population::*mp_SelectFunc)(void);

	/********************************************************************* 
	 *Genetic Functions
	 * 	These functions constitute a collection of functions used in
	 *	implementations of the genetic algorithm.
	 *********************************************************************/
	void crossover( Individual& r_A, /*with:*/ Individual& r_B);
	void crossoverWholeRow( Individual& r_A, /*with:*/ Individual& r_B);
	void mutate( Individual& r_Individual);
	void evolve(/*(SEL)*/ void (Population::*p_SelectMethod)(void));
	void scorePopulation();

	/********************************************************************* 
	 *Parameters 
	 * 	Messages to return instance parameters from a population
	 *********************************************************************/
	int const& size() const;         // Getter   
	void size(int const& newVal);    // Setter
	int const& maxFit() const;         // Getter   
	void maxFit(int const& newVal);    // Setter
	double const& avgFit() const;         // Getter   
	void avgFit(double const& newVal);    // Setter
	int const& sumFit() const;         // Getter   
	void sumFit(int const& newVal);    // Setter
	double const& mutationRate() const;         // Getter   
	void mutationRate(double const& newVal);    // Setter
	int const& totalMutations() const;         // Getter   
	void totalMutations(int const& newVal);    // Setter
	
	/********************************************************************* 
	 *PrintOns 
	 * 	Print individuals, entire populations, statistics, etc.
	 *********************************************************************/
	//- printOn: (IOD)anIOD;
	//- printBestFitOn: (IOD)anIOD;
	//- printReportOn:(IOD)anIOD;
	//- printStatsOn: (IOD)anIOD;
	
};

// Selection function pointer
//void (Population::*mp_selectFunc)(void) = NULL;  
