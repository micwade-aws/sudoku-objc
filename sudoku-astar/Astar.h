 
/* Astar.h
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
#import "Individual.h"

#define SLOT Gene

@interface Astar : Object {
@public
	Individual *puzzle;
	int ni,nj,sub_ni,sub_nj,maxVal;
	int checkCount,nOpen,nClosed;
	int initOpen, initClosed, badGuesses;
	int badGuessFlag, recordStatsFlag;
	double initC, MU;
	IOD runStats;
}

//Setup the parameters of A*
/********************************************************************* 
 *Essential
 * 	These functions will init, free, and run A* search
 *********************************************************************/
- initialize: aPuzzle;
- (int) solve;

/********************************************************************* 
 * Auxiliary
 * 	Supporting functions for the A* search algorithm
 *********************************************************************/
- (int) isSolved;
- (int) isValid: (int*)v;
- scoreOpenSlots;
- (SLOT*) optimalSlot;
- (int)h:(SLOT*)slot;
- (int)g:(SLOT*)slot;

/********************************************************************* 
 * Parameters
 * 	Get/Set/Fetch the value of a member variable
 *********************************************************************/
- (int) checkCount;

/********************************************************************* 
 * Reports
 * 	Printed reports
 *********************************************************************/
- recordOn:(IOD)anIOD;
- printStatsOn:(IOD)anIOD;

@end
