/*
 * Michael Wade
 * Dr. Novobilski
 * CPSC 580 - Machine Learning
 *
 * sudoku.c - single file implementation of Sudoku puzzle creator/solver
 *            for use in CPSC 580.
 *
 * NOTE: I have changed the html write to print to (create if necessary) AnswerGrid.html.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * Problem Definitions
 *
 */

#define GRID_SIZE 	9
#define BOX_SIZE  	3

#define OPEN 		0
#define CLOSED 		1


typedef struct SLOT
{
	int val;
	int status;
	int possibles[9];
	int i,j;
	int score;
	struct SLOT *next; //don't need this anymore
} SLOT;

int badGuessFlag;
static SLOT grid[GRID_SIZE][GRID_SIZE] = { 0 };

/*
 * Prototyping
 *
 */
void showGrid(); 	   	   //better grid display
int g( struct SLOT *slot); //estimated cost from start
int h( struct SLOT *slot); //heuristic function
int findSolution_Astar();  //A* algorithm

/*
 * Code to manage options
 *
 */

static int bSolve = 0;
static int bCreate = 0;
static int bHTML   = 0;
static int  iDiff   = 1;

void usage()
{
	printf( "usage: sudoku {-s -c -h -d:(1...5)} <prob >solution\n" );
	printf( "        -s(olve)\n" );
	printf( "        -c(reate)\n" );
	printf( "        -h(tml)\n" );
	printf( "        -d(ifficulty):<1...5> (1 = easiest)\n" );
	printf( "\n" );
	printf( "Note: -c,-s will create, then solve, a puzzle\n" );
	printf( "\n" );
}


/*
 * Routines to read in, write out, and access grid
 *
 * Format is 9 lines, with 10 chars each 9 (.|[1-9]) and eol
 *
 */

int readGrid( FILE *fp )
{
	int i, j, c;

	for( i=0; i<GRID_SIZE; i++ )
	{
		for( j=0; j<GRID_SIZE; j++ )
		{
			c = fgetc( fp );
			if ( c == '.' )
				grid[i][j].val = 0;
			else
			{
				c = c - '0';
				if ( (c < 1) || (c > 9) )
					return 0;
				grid[i][j].val = c;
				grid[i][j].status = CLOSED;
			}
		}
		fgetc(fp);
	}
	return 1;
}

int writeGrid( FILE *fp )
{
	int i, j;

	//for diplay right now..
	showGrid();
	return 1;

	for( i=0; i<GRID_SIZE; i++ )
	{
		for( j=0; j< GRID_SIZE; j++ )
			fputc( grid[i][j].val ? (grid[i][j].val+'0') : '.', fp );
		fputc( '\n', fp );
	}
	return 1;
}


//TODO: determine if the DOS shell has color codes
//terminal color codes (UNIX):
const char *const green = "\033[0;40;32m";
const char *const red = "\033[0;40;31m";
const char *const blue = "\033[0;40;34m";
const char *const normal = "\033[0m";

void showGrid()
{
	int i, j;

	for( i=0; i<GRID_SIZE; i++ )
	{
		printf("%i: ", i+1);
		for( j=0; j<GRID_SIZE; j++ )
				printf("%u ",  grid[i][j].val); //show slot by value
		printf("\n");
	}
}


/*
 * Rountine to validate puzzle
 *
 * AJN - Changed isSolved and isValid to return 1 if solution,
 *       -1 if legal but not solved and 0 if illegal.
 *
 */

int isValid( int v[] )
{
	int i, s, a[GRID_SIZE];

	for( i=0; i<GRID_SIZE; i++ )
		a[i] = 0;

	for( i=0; i<GRID_SIZE; i++ )
		if ( v[i] )
			a[v[i]-1]++;

	for( i=0, s=0; i<GRID_SIZE; i++ )
		if ( a[i] > 1 )
			return 0;
		else if ( a[i] )
			s += a[i];

	return( s == 9 ? 1 : -1 );
}

static long int checkCount = 0;

int isSolved()
{
	int i, j, k, l, v[GRID_SIZE];
	int c1, c2, c3;

	checkCount++;
	if ( !(checkCount % 1000000) )
		printf( "%ld, ", checkCount );

	for( i=0; i<GRID_SIZE; i++ ) // check rows
	{
		for( j=0; j<GRID_SIZE; j++ )
			v[j] = grid[i][j].val;
		if ( !(c1 = isValid( v )) )
			return( 0 );
	}

	for( j=0; j<GRID_SIZE; j++ ) // check columns
	{
		for( i=0; i<GRID_SIZE; i++ )
			v[i] = grid[i][j].val;
		if ( !(c2 = isValid( v )) )
			return( 0 );
	}

	for( i=0; i<BOX_SIZE; i++ ) // check boxes
		for( j=0; j<BOX_SIZE; j++ )
		{
			for( k=0; k<BOX_SIZE; k++ )
				for( l=0; l<BOX_SIZE; l++ )
					v[k*BOX_SIZE+l] = grid[i*BOX_SIZE+k][j*BOX_SIZE+l].val;
			if ( !(c3 = isValid( v )) )
				return( 0 );
		}

	return( (c1+c2+c3) == BOX_SIZE ? 1 : -1 );
}


void makeMagicHappen( int A, int B ){
	int i,j;

	for(i=0;i<GRID_SIZE;i++)
		for(j=0;j<GRID_SIZE;j++)
			grid[i][j].val = ((i - (A*j)) % GRID_SIZE);
	
	return;
}

void showMeTheMagic()
{
	showGrid();
	return;
}


int main( int argc, char *argv[] )
{
	//Get A and B
	int A,B;

	A = atoi(argv[1]);
	B = atoi(argv[2]);

	makeMagicHappen(A,B);

	showMeTheMagic();


	return( 0 );
}



