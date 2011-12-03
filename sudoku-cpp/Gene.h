/* Gene.h
 * Gene class implementation file.
 *
 * Michael J Wade 
 * mike@iammyownorg.org 
 * Copyright (c) 2010
 * 
 */
#pragma once

class Gene
{
public:
	Gene(void);
	~Gene(void);
	
	int allele;
	int status;
	int xLoc, yLoc, Loc;			
	int *possibles;
	double score; //Astar specific score value
	int suggested; //This is the value "suggested" by the SGA :)
};
