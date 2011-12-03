/* Astar.m
 *
 * Michael J Wade 
 * mike@iammyownorg.org 
 * Copyright (c) 2007
 * 
 */

#import "Astar.h"
#import "Individual.h"

//Map the Individual class to the Astar terminology.
#define SLOT Gene
#define GRID puzzle->chromosone
#define DEBUG printf

@implementation Astar

- initialize: aPuzzle {
	puzzle = aPuzzle;
	ni = puzzle->ni;
	nj = puzzle->nj;
	sub_ni = puzzle->sub_ni;
	sub_nj = puzzle->sub_nj;
	maxVal = puzzle->maxAllele;	
	checkCount = 0;
	nOpen = 0;
	nClosed = 0;
	badGuessFlag = 0;
	badGuesses = 0;
	initOpen = [puzzle open];
	initClosed = [puzzle closed];
	initC = [puzzle MU];
	MU = [puzzle MU];
	recordStatsFlag = 0;	
	return self;
}

//Pre: none
//Post: Returns 1 if vector v is valid.  Otherwise, -1.
- (int) isValid: (int*)v
{
	int i, s, *a;
	a = (int*)objc_malloc(maxVal*sizeof(int));

	for( i=0; i<maxVal; i++ )
		a[i] = 0;

	for( i=0; i<maxVal; i++ )
		if ( v[i] )
			a[v[i]-1]++;

	for( i=0, s=0; i<maxVal; i++ )
		if ( a[i] > 1 )
			return -1; //was 0  //Indicate an invalid vector.
		else if ( a[i] )
			s += a[i];

	objc_free(a);
	return( s == maxVal ? 1 : 0 ); //was -1
}


- (int) isSolved
{
	int i, j, m, n, w, *v;
	int c1, c2, c3;

	//Every call to isSolved increases the checkCount
	checkCount++;
	//if(!(checkCount % 10000))
	//	[puzzle printOn:(stdout)];
	//	[self printStatsOn:(stdout)];
	
	//Allocate a vector that will be used to validate 
	//each rule (row,col,box)
	v = (int*)objc_malloc(maxVal*sizeof(int));
	
	//Perform checks for each rule.
	
	for( j=0; j < nj; j++ ) // check rows
	{
		for( i=0; i < ni; i++ )
			v[i] = GRID[nj*i + j].allele;
		if ( !(c1 = [self isValid:v]) )
			return( 0 );
	}

	for( i=0; i<ni; i++ ) // check columns
	{
		for( j=0; j<nj; j++ )
			v[j] = GRID[nj*i + j].allele;
		if ( !(c2 = [self isValid:v]) )
			return( 0 );
	}
	
	for( n=0; n<sub_nj; n++ ) // check boxes
		for( m=0; m<sub_ni; m++ )
		{
			w = 0;
			for( j=0; j<sub_ni; j++ )
				for( i=0; i<sub_nj; i++ ){
					v[w] = 
					GRID[(nj*( (m*sub_ni)+i) ) + 
						       ( (n*sub_nj)+j )].allele;
					w++;
				}
			if ( !(c3 = [self isValid:v]) )
				return( 0 );
		}

	objc_free(v);
	//new, check for invalid puzzle
	if(c1 == -1 || c2 == -1 || c3 == -1)
		return -1;

	return( (c1+c2+c3) == 3 ? 1 : 0 ); //was 3 1 -1
}


/*
 * scoreOpenSlots - Assigns scores to all open slots.
 *
 */
- scoreOpenSlots
{
	SLOT *slot = NULL;
	int i,j;

	for(j=0; j < nj; j++)
		for(i=0; i<ni; i++)
			if( GRID[nj*i + j].status == OPEN )
			{
				slot = &GRID[nj*i + j];
				//slot->score = 1;
				//slot->score = [self h:slot] + 1;
				//slot->score = [self g:slot];
				//slot->score = [self h:slot] + [self g:slot];
				//slot->score = [self h:slot] + [puzzle certaintyOf:(slot)];
				slot->score = [puzzle certaintyOf:(slot)]; //problem?
			}

	//printScores();
	return self;
}

/*
 * g(n) - Estimates how much closer this slot will bring us to finishing the puzzle.
 *
 */
- (int) g:(SLOT*)slot
{
	int i,j,h,g;
	int iOffset, jOffset;

	g = maxVal*3;

	//eliminate possible row values
	j = slot->yLoc;
	for(i=0; i<ni; i++)
		if(GRID[nj*i+j].status == OPEN)
			g--;

	//eliminate possible col values
	i = slot->xLoc;
	for(j=0; j<nj; j++)
		if(GRID[nj*i+j].status == OPEN)
			g--;

	//eliminate possible box values (this is tricky)
	iOffset = slot->xLoc - (slot->xLoc % sub_ni);
	jOffset = slot->yLoc - (slot->yLoc % sub_nj);
	
	for( j=jOffset; j<(jOffset+sub_nj); j++ )
		for( i=iOffset; i<(iOffset+sub_ni); i++ )
			if( GRID[nj*i + j].status == OPEN )
				g--;
	return g;
}

/*
 * h(n) - Estimates the score of this slot given the current puzzle.
 *
 */
- (int) h:(SLOT*)slot
{
	int i,j,h;
	int iOffset, jOffset;

	//(Re)Set possibles Array
	for(i=0; i < ni; i++)
		slot->possibles[i] = i+1;

	//eliminate possible row values
	j = slot->yLoc;
	for(i=0; i<ni; i++)
		slot->possibles[(GRID[nj*i+j].allele - 1)] = 0;

	//eliminate possible col values
	i = slot->xLoc;
	for(j=0; j<nj; j++)
		slot->possibles[(GRID[nj*i+j].allele - 1)] = 0;

	//eliminate possible box values (this is tricky)
	/*BOXES:
	*	A | B | C
	*	D | E | F
	*	G | H | I
	*/
	iOffset = slot->xLoc - (slot->xLoc % sub_ni);
	jOffset = slot->yLoc - (slot->yLoc % sub_nj);
	
	for( j=jOffset; j<(jOffset+sub_nj); j++ )
		for( i=iOffset; i<(iOffset+sub_ni); i++ )
			if( GRID[nj*i + j].status == CLOSED )
				slot->possibles[(GRID[nj*i+j].allele - 1)] = 0;

	//Compute the score based on the remaining possible values.
	h=0;
	for(i=0; i < maxVal; i++)
	{
		if(slot->possibles[i] != 0)
			h++;
	}

	if( h == 0 )
	{
		h = maxVal; //impossible, set to maxVal to show the algorithm that this is not obtainable
		badGuessFlag = 1;
	}

	return h;
	
}

/*
 * optimalSlot() - Returns the slot with the lowest score..
 *
 * NOTE: At first, I reversed the grid's traversal below to help me keep track of
 *		 the optimal slot return in the grid.. (Take the best score nearest the top.)
 *
 *		 However, it appears that it *MAY* takes advantage of the way the puzzles are created
 *	  	 Changing the traversal back to 0,0 to 8,8 will break my code's ability to
 *		 solve in n time (on easy puzzles) and occasionally it's ability to
 *		 solve at all.  This will apply to any puzzle, even puzzles from off the net.
 *		 It might also be related to the fact that I am taking the lowest value from the list
 *		 of possibles when guessing.
 *
 */
- (SLOT*) optimalSlot
{

	SLOT *lowestSlot = NULL;
	double lowestScore = 999;//used to be 999
	int i,j;

	//[puzzle printCertainty:(stdout)];
	for(j=(nj-1); j >= 0; j--)
		for(i=(ni-1); i >= 0; i--){
			if( GRID[nj*i+j].status == OPEN && GRID[nj*i+j].score < lowestScore)
			{
				lowestSlot = &(GRID[nj*i+j]);
				lowestScore = GRID[nj*i+j].score;
			}
			else if(lowestSlot == NULL && GRID[nj*i+j].status == OPEN){
				lowestSlot = &GRID[nj*i+j];
				lowestScore = GRID[nj*i+j].score;
			}
		}

	//printf("lowest slot @ (%d,%d)\n",lowestSlot->xLoc,lowestSlot->yLoc);
	return lowestSlot;
}

/*
 * findSolution_Astar() - Recursive implementation of A*.
 *
 */
- (int) solve {
	
	int i,loc,val,solved;
	SLOT *slot = NULL;

	//Record Run Stats
	if(recordStatsFlag)
		[self printStatsOn:runStats];
	//[self printStatsOn:stdout];

	//Is it solved? so soon?
	solved = [self isSolved];
	if ( solved == 1 )
	{
		//printf("solved..");
		return 1;
	}
	else if ( solved == -1 )
		return 0;

	//Score the open slots
	[self scoreOpenSlots];

	//Need to backtrack?
	/*if( badGuessFlag == 1 )
	{
		//printf("\nBAD GUESS..returning 0");
		badGuessFlag = 0;
		badGuesses++;
		return 0;
	}*/

	//Find the slot with lowest score (optimal).
	slot = [self optimalSlot];

	//error check the slot pointer
	if ( slot == NULL)
	{
		printf("\nbad optimal slot!\n");
		[puzzle printCertainty:(stdout)];
		return 1; //must not be any more open slots..
	}

	loc = slot->Loc;
#if 1
	//Loop through the possible values for the slot.
	while([puzzle moveRemainsAt:(loc)])
	{
		//Try this value (Take a guess)
		//printf("loc: %d\n",loc);
		val = [puzzle mostLikelyAt:(loc)];
		[puzzle gene:(loc) allele: val ];
		//DEBUG("(%d)->allele: %d\n",loc,slot->allele);

		//Close the slot
		slot->status = CLOSED;
		nOpen--;
		nClosed++;

		//Keep searching?
		if( [self solve] )
		{
			return 1;
			printf("A* returned");
		}
		else
		{
			//slot->allele = 0;
			[puzzle gene:(loc) allele:0 ];//place back on the open list
			slot->status = OPEN;
			nOpen++;
			nClosed--;
		}
	}
#endif

#if 0
	//Loop through the possible values for the slot.
	//for(i=0; i<maxVal; i++)
	for(i=(maxVal-1); i>=0; i--)
	{

		//Valid possible value?
		if ( slot->possibles[i] > 0 && slot->status == OPEN)
		//if([puzzle moveRemainsAt:(loc)])
		{
			//Try this value
			//slot->allele = slot->possibles[i];
			//[puzzle gene:(loc) allele: (slot->possibles[i]) ];
			[puzzle gene:(loc) allele: (slot->possibles[i]) ];
			//printf("\nslot:%d forced: %d, mostLikely:%d\n",loc,slot->allele,[puzzle mostLikelyAt:(loc)]);
			

			//Close the slot
			slot->status = CLOSED;
			nOpen--;
			nClosed++;

			//Keep searching.
			if( [self solve] )
			{
				return 1;
				printf("A* returned");
			}
			else
			{
				//slot->allele = 0;
				[puzzle gene:(loc) allele: 0 ];
				slot->status = OPEN;
				nOpen++;
				nClosed--;
			}
		}
	}
#endif
	//Reaching this point indicates a lack of potential moves.
	return 0;
}

- (int) checkCount {
	return checkCount;
}

- recordOn:(IOD)anIOD {
	recordStatsFlag = 1;
	runStats = anIOD;
	return self;
}

//Will print statistics of given state.
- printStatsOn:(IOD)anIOD{
	/*
	 * init. open | init closed | init C | # checks | C | open | closed | bad guesses
	 */
	fprintf(anIOD,"%d %d %f %d %f %d %d %d %d", initOpen, initClosed, initC, checkCount, [puzzle MU], [puzzle open], [puzzle closed], badGuesses, [puzzle evalFitness]);
	return self;
}

@end
