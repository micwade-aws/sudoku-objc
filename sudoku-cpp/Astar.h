 /* Astar.h
 *
 * Michael J Wade 
 * mike@iammyownorg.org 
 * Copyright (c) 2011
 * 
 */
#pragma once
#include <stdio.h>
#include "Individual.h"

#define SLOT Gene

class Astar {
public:
	Astar();
	Astar( Individual& rPuzzle);
	~Astar();

	Individual* pPuzzle;
	int ni,nj,sub_ni,sub_nj,maxVal;
	int checkCount,nOpen,nClosed;
	int initOpen, initClosed, badGuesses;
	int badGuessFlag, recordStatsFlag;
	double initC, MU;
	FILE runStats;

	//Main routine A*
	int solve();

	//Auxiliary Functions
	int isSolved();
	int isValid(int* v);
	void scoreOpenSlots();
	SLOT* optimalSlot();
	int h(SLOT* slot);
	int g(SLOT* slot);
	//- recordOn:(IOD)anIOD;

	//Variable Access
	//int checkCount();

//- printStatsOn:(IOD)anIOD;

};
