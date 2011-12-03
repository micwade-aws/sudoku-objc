 
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
- initWith: aPuzzle;
//Main routine A*
- (int) solve;

//Auxiliary Functions
- (int) isSolved;
- (int) isValid: (int*)v;
- scoreOpenSlots;
- (SLOT*) optimalSlot;
- (int)h:(SLOT*)slot;
- (int)g:(SLOT*)slot;
- recordOn:(IOD)anIOD;

//Variable Access
- (int) checkCount;

- printStatsOn:(IOD)anIOD;

@end
