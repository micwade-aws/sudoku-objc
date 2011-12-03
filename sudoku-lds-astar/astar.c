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
static int  iDiff   = 0;

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

int parseArgs( int c, char *v[] )
{
	int i;

	if ( c == 1 )
	{
		usage();
		return 0;
	}

	for( i=1; i<c; i++ )
	{
		switch( v[i][1] )
		{
			case 's':
				bSolve = 1;
				break;

			case 'c':
				bCreate = 1;
				break;

			case 'h':
				bHTML = 1;
				break;

			case 'd':
				iDiff = v[i][3] - '0';
				if ( (iDiff < 1) || (iDiff > 5) )
				{
					printf( "%d not valid for -d:\n", iDiff );
					usage();
					return 0;
				}
				break;

			default:
				printf( "-%c is illegal option\n", v[i][1] );
				usage();
				return 0;
		}
	}

	return 1;
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
                                if ( (c < 1) || (c > GRID_SIZE) )
                                {
                                        j--; 
                                }
                                else{
                                        grid[i][j].val = c;
                                        grid[i][j].status = CLOSED;
                                }
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
 * Routines to display grid in HTML
 *
 */

int writeSubTable( FILE *fp, int r, int c, char *bg )
{
	int i, n, j, m;

	n = r+3;
	m = c+3;

	fprintf( fp, "<TABLE border=2 BGCOLOR=\"%s\">\n", bg );
	fprintf( fp, "<TBODY>\n" );
	for( i=r; i<n; i++ )
	{
		fprintf( fp, "<TR>\n" );
		for( j=c; j<m; j++ )
		{
			fprintf( fp, "<TD>\n" );
			fprintf( fp, "%c", grid[i][j].val ? (grid[i][j].val+'0') : '.' );
			fprintf( fp, "</TD>\n" );
		}
		fprintf( fp, "</TR>\n" );
	}
	fprintf( fp, "</TBODY>\n" );
	fprintf( fp, "</TABLE>\n" );

	return 1;
}

int writeHTMLPuzzle( FILE *fp )
{
	int i, j;

	fprintf( fp, "<HTML>\n" );
	fprintf( fp, "<BODY BGCOLOR=\"white\">\n" );

	fprintf( fp, "<TABLE border=3>\n" );
	fprintf( fp, "<TBODY>\n" );
	for( i=0; i<3; i++ )
	{
		fprintf( fp, "<TR>\n" );
		for( j=0; j<3; j++ )
		{
			fprintf( fp, "<TD>\n" );
			writeSubTable( fp, i*3, j*3, ( (i+j)%2 ? "white" : "grey" ) );
			fprintf( fp, "</TD>\n" );
		}
		fprintf( fp, "</TR>\n" );
	}
	fprintf( fp, "</TBODY>\n" );
	fprintf( fp, "</TABLE>\n" );

	fprintf( fp, "   </BODY>\n" );
	fprintf( fp, "</HTML>\n" );
	return( 1 );
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

/*
 * Routines to solve puzzle
 *
 */

/*
 *     A* FUNCTIONS
 */

// A* globals
int createList = 0;
int countOpen = 0;

/*
 * createOpenList - Sets status of each slot.  If slot.val = 0 then OPEN.
 *
 */
void createOpenList()
{
	int i,j;
	showGrid();
	for( i=0; i<GRID_SIZE; i++ )
		for( j=0; j<GRID_SIZE; j++ )
		{
			if (grid[i][j].val == 0)
			{
				grid[i][j].i = i;
				grid[i][j].j = j;
				grid[i][j].status = OPEN;
				countOpen++;
				//printf("opening: [%u][%u]\n",i,j);
			}
			else
				grid[i][j].status = CLOSED;
		}
}

/*
 * printScores - debugging function..
 *
 */
void printScores()
{
	int i,j;

	printf("# open: %u\n",countOpen);
	printf("scores: %u");
	for( i=0; i<GRID_SIZE; i++ )
		for( j=0; j<GRID_SIZE; j++ )
			if (grid[i][j].status == OPEN)
				printf("[%u][%u] = %u \n",i,j, grid[i][j].score);

	printf("\n");
}

/*
 * scoreOpenSlots - Assigns scores to all open slots.
 *
 */
void scoreOpenSlots()
{
	struct SLOT *slot = NULL;
	int i,j;

	for(i=0; i < GRID_SIZE; i++)
		for(j=0; j<GRID_SIZE; j++)
			if( grid[i][j].status == OPEN )
			{
				slot = &grid[i][j];
				slot->score = g(slot) + h(slot);
			}

	//printScores();

}

/*
 * g(n) - Estimates how much closer this slot will bring us to finishing the puzzle.
 *
 */
int g( struct SLOT *slot)
{
	int i,j,k,l,g;

	//return 0;

	//Attempt to use a measure of how connected this slot is to the puzzle
	//by measuring the number of affected nodes.

	//The less connected g is to the other slots
	//the stronger correlation to the slot being near the solution
	//Hence, if there are 8+8+6 open slots connected to g, then g is far away

	g = 8+8+6; //possible slots effected by g

		//eliminate possible row values
		i = slot->i;
		for(j=0; j<GRID_SIZE; j++)
			if( grid[i][j].status == OPEN )
				g--;


		//eliminate possible col values
		j = slot->j;
		for(i=0; i<GRID_SIZE; i++)
			if( grid[i][j].status == OPEN )
				g--;

		//eliminate box connections (this is tricky)
		/*BOXES:
		*	A | B | C
		*	D | E | F
		*	G | H | I
		*/
		iOffset = gene->xLoc - (gene->xLoc % sub_ni);
		jOffset = gene->yLoc - (gene->yLoc % sub_nj);
	
		for( j=jOffset; j<(jOffset+sub_nj); j++ )
			for( i=iOffset; i<(iOffset+sub_ni); i++ )
				if( chromosone[nj*i + j].status == CLOSED )
					a[(chromosone[nj*i + j].allele)-1]++;

		if( slot->i <= 2 )
		{
			if( slot->j <= 2 )
			{
				//BOX: A
				for( k=0; k<BOX_SIZE; k++ )
					for( l=0; l<BOX_SIZE; l++ )
						if( grid[k][l].status == OPEN )
							g--;
			}
			else if( slot->j <= 5 )
			{
				//BOX: B
				for( k=0; k<BOX_SIZE; k++ )
					for( l=BOX_SIZE; l<(BOX_SIZE*2); l++ )
						if( grid[k][l].status == OPEN )
							g--;
			}
			else //slot->j > 7
			{
				//BOX: C
				for( k=0; k<BOX_SIZE; k++ )
					for( l=BOX_SIZE*2; l<(BOX_SIZE*3); l++ )
						if( grid[k][l].status == OPEN )
							g--;
			}
		}
		else if( slot->i <= 5 )
		{
			if( slot->j <= 2 )
			{
				//BOX: D
				for( k=BOX_SIZE; k<(BOX_SIZE*2); k++ )
					for( l=0; l<BOX_SIZE; l++ )
						if( grid[k][l].status == OPEN )
							g--;
			}
			else if( slot->j <= 5 )
			{
				//BOX: E
				for( k=BOX_SIZE; k<(BOX_SIZE*2); k++ )
					for( l=BOX_SIZE; l<(BOX_SIZE*2); l++ )
						if( grid[k][l].status == OPEN )
							g--;
			}
			else //slot->j > 7
			{
				//BOX: F
				for( k=BOX_SIZE; k<(BOX_SIZE*2); k++ )
					for( l=BOX_SIZE*2; l<(BOX_SIZE*3); l++ )
						if( grid[k][l].status == OPEN )
							g--;
			}
		}
		else //slot->i > 7
		{
			if( slot->j <= 2 )
			{
				//BOX: G
				for( k=(BOX_SIZE*2); k<(BOX_SIZE*3); k++ )
					for( l=0; l<BOX_SIZE; l++ )
						if( grid[k][l].status == OPEN )
							g--;
			}
			else if( slot->j <= 5 )
			{
				//BOX: H
				for( k=(BOX_SIZE*2); k<(BOX_SIZE*3); k++ )
					for( l=BOX_SIZE; l<(BOX_SIZE*2); l++ )
						if( grid[k][l].status == OPEN )
							g--;
			}
			else //slot->j > 7
			{
				//BOX: H
				for( k=(BOX_SIZE*2); k<(BOX_SIZE*3); k++ )
					for( l=BOX_SIZE*2; l<(BOX_SIZE*3); l++ )
						if( grid[k][l].status == OPEN )
							g--;
			}
		}


	return g;
}

/*
 * h(n) - Estimates the score of this slot given the current puzzle.
 *
 */
int h( struct SLOT *slot)
{
	int i,j,k,l,h;

	//reset possibles
	for(i=0; i < GRID_SIZE; i++)
		slot->possibles[i] = i+1;

	//eliminate possible row values
	i = slot->i;
	for(j=0; j<GRID_SIZE; j++)
		slot->possibles[(grid[i][j].val - 1)] = 0;

	//eliminate possible col values
	j = slot->j;
	for(i=0; i<GRID_SIZE; i++)
		slot->possibles[(grid[i][j].val - 1)] = 0;

	//eliminate possible box values (this is tricky)
	/*BOXES:
	*	A | B | C
	*	D | E | F
	*	G | H | I
	*/
	if( slot->i <= 2 )
	{
		if( slot->j <= 2 )
		{
			//BOX: A
			for( k=0; k<BOX_SIZE; k++ )
				for( l=0; l<BOX_SIZE; l++ )
					slot->possibles[(grid[k][l].val - 1)] = 0;
		}
		else if( slot->j <= 5 )
		{
			//BOX: B
			for( k=0; k<BOX_SIZE; k++ )
				for( l=BOX_SIZE; l<(BOX_SIZE*2); l++ )
					slot->possibles[(grid[k][l].val - 1)] = 0;
		}
		else //slot->j > 7
		{
			//BOX: C
			for( k=0; k<BOX_SIZE; k++ )
				for( l=BOX_SIZE*2; l<(BOX_SIZE*3); l++ )
					slot->possibles[(grid[k][l].val - 1)] = 0;
		}
	}
	else if( slot->i <= 5 )
	{
		if( slot->j <= 2 )
		{
			//BOX: D
			for( k=BOX_SIZE; k<(BOX_SIZE*2); k++ )
				for( l=0; l<BOX_SIZE; l++ )
					slot->possibles[(grid[k][l].val - 1)] = 0;
		}
		else if( slot->j <= 5 )
		{
			//BOX: E
			for( k=BOX_SIZE; k<(BOX_SIZE*2); k++ )
				for( l=BOX_SIZE; l<(BOX_SIZE*2); l++ )
					slot->possibles[(grid[k][l].val - 1)] = 0;
		}
		else //slot->j > 7
		{
			//BOX: F
			for( k=BOX_SIZE; k<(BOX_SIZE*2); k++ )
				for( l=BOX_SIZE*2; l<(BOX_SIZE*3); l++ )
					slot->possibles[(grid[k][l].val - 1)] = 0;
		}
	}
	else //slot->i > 7
	{
		if( slot->j <= 2 )
		{
			//BOX: G
			for( k=(BOX_SIZE*2); k<(BOX_SIZE*3); k++ )
				for( l=0; l<BOX_SIZE; l++ )
					slot->possibles[(grid[k][l].val - 1)] = 0;
		}
		else if( slot->j <= 5 )
		{
			//BOX: H
			for( k=(BOX_SIZE*2); k<(BOX_SIZE*3); k++ )
				for( l=BOX_SIZE; l<(BOX_SIZE*2); l++ )
					slot->possibles[(grid[k][l].val - 1)] = 0;
		}
		else //slot->j > 7
		{
			//BOX: H
			for( k=(BOX_SIZE*2); k<(BOX_SIZE*3); k++ )
				for( l=BOX_SIZE*2; l<(BOX_SIZE*3); l++ )
					slot->possibles[(grid[k][l].val - 1)] = 0;
		}
	}

	//count the remaining possibles and return as heuristic.
	h=0;
	for(i=0; i < GRID_SIZE; i++)
	{
		if(slot->possibles[i] != 0)
			h++;
	}

	if( h == 0 )
	{
		h = 9; //impossible, set to 9 to show the algorithm that this is not obtainable
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
SLOT *optimalSlot()
{

	struct SLOT *lowestSlot = NULL;
	int lowestScore = 999;
	int i,j;

	for(i=(GRID_SIZE-1); i >= 0; i--)
		for(j=(GRID_SIZE-1); j >= 0; j--)
			if( grid[i][j].status == OPEN && grid[i][j].score < lowestScore)
				lowestSlot = &grid[i][j];

	return lowestSlot;
}

/*
 * findSolution_Astar() - Recursive implementation of A*.
 *
 */
int findSolution_Astar()
{
	char c;
	int i,solved;
	struct SLOT *slot = NULL;

	//If not already, setup the OPEN list
	if (!createList){
		createOpenList();
		createList = 1;
		printf("# Open: %u\n\nShow intermitent girds (y/n)?\n", countOpen);
		c = fgetc(stdin);
	}

	//Is it solved? so soon?
	solved = isSolved();
	if ( solved == 1 )
	{
		//printf("solved..");
		return 1;
	}
	else if ( solved == 0 )
		return 0;

	//Redisplay the grid
	if( c == 'y' || c == 'Y' )
		showGrid();

	//Score the open slots
	scoreOpenSlots();

	//Need to backtrack?
	if( badGuessFlag == 1 )
	{
		printf("\nBAD GUESS..returning 0");
		badGuessFlag = 0;
		return 0;
	}

	//Find the slot with lowest score (optimal).
	slot = optimalSlot();

	//error check the slot pointer
	if ( slot == NULL)
	{
		//printf("\nbad optimal slot!\n");
		return 1; //must not be any more open slots..
	}


	//Loop through the possible values for the slot.
	for(i=0; i<GRID_SIZE; i++)
	{
		//Valid possible value?
		if ( slot->possibles[i] > 0 && slot->status == OPEN)
		{
			//Try this value
			slot->val = slot->possibles[i];

			//Close the slot
			slot->status = CLOSED;
			countOpen--;

			//Keep searching.
			if( findSolution_Astar() )
			{
				return 1;
				printf("A* returned");//return 1;
			}
			else
			{
				slot->val = 0;
				countOpen++;
				slot->status = OPEN;
			}
		}
	}

	//Reaching this point indicates a lack of potential moves.
	return 0;
}

/*
 *     Depth First Search
 */
int findSolution_DFS()
{
	int i, j, k;
	int check;

	check = isSolved();
	if ( check == 1 )
		return 1;
	else if ( check == 0 )
		return 0;

	for( i=0; i<GRID_SIZE; i++ )
		for( j=0; j<GRID_SIZE; j++ )
		{
			if ( !grid[i][j].val )
			{
				for( k=1; k<=GRID_SIZE; k++ )
				{
					grid[i][j].val = k;
					if ( findSolution_DFS() )
						return 1;
				}
				grid[i][j].val = 0;
				return 0;
			}
		}

	return( 0 );
}

int solvePuzzle()
{
	return( findSolution_DFS() );
	//return( findSolution_Astar() );
}

/*
 * Routines to create puzzle
 *
 */

int randInt( int f, int l )
{
	//UNIX:
	return (f + (double)rand () * (l - f + 1) / RAND_MAX);

	//WINDOWS:
	/*int r, d;

	d = l-f+1;
	if ( d == 1 )
		return f;

	r = rand();
	return( f + (int)(((double)r/(1+RAND_MAX))*d) );
*/
}

void createNewPuzzle( int diff )
{
	int r, c, i, n;
	static int seed[] = { 0, 3, 6, 1, 4, 7, 2, 5, 8 };

	for( c=0; c<9; c++ )
		for( r=0; r<9; r++ )
			grid[r][c].val = ( ( seed[c] + r ) % 9) + 1;

	n = 1<<(2+diff);
	for( i=0; i<n; i++ )
		grid[randInt(0,8)][randInt(0,8)].val = 0;
}


/*
 * main routine
 *
 */

int main( int argc, char *argv[] )
{
	
#if 1
	if ( !parseArgs( argc, argv ) )
		exit( 1 );


	if (  !bCreate )
	{
		//if ( !readGrid( fopen("test.grid", "r") ) ) //testing grid from the net.
		if ( !readGrid( stdin ) )
		{
			fprintf( stderr, "error: bad problem input\n" );
			exit( 1 );
		}
	}
	else
		createNewPuzzle( iDiff );

	if ( bSolve )
	{
		if ( !solvePuzzle() )
		{
			fprintf( stderr, "error: puzzle is unsolveable\n" );
			exit( 1 );
		}
		//printf( "Check Count: %ld\n", checkCount );
		printf( "%d\n", checkCount );
	}
#if DEBUG
	if ( bHTML )
		writeHTMLPuzzle( fopen("AnswerGrid.html", "w+") ); //to the command prompt??
	else
		writeGrid( stdout );
#endif

#else
	
//main() - FOR TESTING WITH SAMPLE GRIDS:

int i;
	char *puzzles[82];
	FILE *fp;
	puzzles[0] = "p00.pz";
	puzzles[1] = "p01.pz";
	puzzles[2] = "p02.pz";
	puzzles[3] = "p03.pz";
	puzzles[4] = "p04.pz";
	puzzles[5] = "p05.pz";
	puzzles[6] = "p06.pz";
	puzzles[7] = "p07.pz";
	puzzles[8] = "p08.pz";
	puzzles[9] = "p09.pz";
	puzzles[10] = "p10.pz";
	puzzles[11] = "p11.pz";
	puzzles[12] = "p12.pz";
	puzzles[13] = "p13.pz";
	puzzles[14] = "p14.pz";
	puzzles[15] = "p15.pz";
	puzzles[16] = "p16.pz";
	puzzles[17] = "p17.pz";
	puzzles[18] = "p18.pz";
	puzzles[19] = "p19.pz";
	puzzles[20] = "p20.pz";
	puzzles[21] = "p21.pz";
	puzzles[22] = "p22.pz";
	puzzles[23] = "p23.pz";
	puzzles[24] = "p24.pz";
	puzzles[25] = "p25.pz";
	puzzles[26] = "p26.pz";
	puzzles[27] = "p27.pz";
	puzzles[28] = "p28.pz";
	puzzles[29] = "p29.pz";
	puzzles[30] = "p30.pz";
	puzzles[31] = "p31.pz";
	puzzles[32] = "p32.pz";
	puzzles[33] = "p33.pz";
	puzzles[34] = "p34.pz";
	puzzles[35] = "p35.pz";
	puzzles[36] = "p36.pz";
	puzzles[37] = "p37.pz";
	puzzles[38] = "p38.pz";
	puzzles[39] = "p39.pz";
	puzzles[40] = "p40.pz";
	puzzles[41] = "p41.pz";
	puzzles[42] = "p42.pz";
	puzzles[43] = "p43.pz";
	puzzles[44] = "p44.pz";
	puzzles[45] = "p45.pz";
	puzzles[46] = "p46.pz";
	puzzles[47] = "p47.pz";
	puzzles[48] = "p48.pz";
	puzzles[49] = "p49.pz";
	puzzles[50] = "p50.pz";
	puzzles[51] = "p51.pz";
	puzzles[52] = "p52.pz";
	puzzles[53] = "p53.pz";
	puzzles[54] = "p54.pz";
	puzzles[55] = "p55.pz";
	puzzles[56] = "p56.pz";
	puzzles[57] = "p57.pz";
	puzzles[58] = "p58.pz";
	puzzles[59] = "p59.pz";
	puzzles[60] = "p60.pz";
	puzzles[61] = "p61.pz";
	puzzles[62] = "p62.pz";
	puzzles[63] = "p63.pz";
	puzzles[64] = "p64.pz";
	puzzles[65] = "p65.pz";
	puzzles[66] = "p66.pz";
	puzzles[67] = "p67.pz";
	puzzles[68] = "p68.pz";
	puzzles[69] = "p69.pz";
	puzzles[70] = "p70.pz";
	puzzles[71] = "p71.pz";
	puzzles[72] = "p72.pz";
	puzzles[73] = "p73.pz";
	puzzles[74] = "p74.pz";
	puzzles[75] = "p75.pz";
	puzzles[76] = "p76.pz";
	puzzles[77] = "p77.pz";
	puzzles[78] = "p78.pz";
	puzzles[79] = "p79.pz";
	puzzles[80] = "p80.pz";
	puzzles[81] = "p81.pz";


	fp = fopen("resultsDFS.csv", "w");
	fprintf(fp, "missing,count\n");

	for(i=0;i<82;i++)
	{
		readGrid( fopen(puzzles[i], "r") );
		showGrid();
		if ( !solvePuzzle() )
		{
			fprintf( fp, "error: puzzle is unsolveable\n" );
			exit( 1 );
		}
		fprintf( fp, "%i,%i\n",i, checkCount );
		createList = 0;
		checkCount = 0;
		badGuessFlag = 0;
		countOpen = 0;
	}

#endif

	return 0;
}



