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
#include <math.h>

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
double randReal( double low, double high);

/*
 * Code to manage options
 *
 */

static int bSolve = 0;
static int bCreate = 0;
static int bHTML   = 0;
static int  iDiff   = 0;

//Other variables (for the algorithms)
int openCount = 0;
int createList = 0;
int countOpen = 0;

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
			{
				grid[i][j].val = 0;
				grid[i][j].status = OPEN;
			}
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

	return( s == 9 ? 1 : 0 );
}

static long int checkCount = 0;

int isSolved()
{
	int i, j, k, l, v[GRID_SIZE];
	int c1, c2, c3;

	checkCount++;
	if ( !(checkCount % 100) )
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

	return( (c1+c2+c3) == BOX_SIZE ? 1 : 0 );
}

/*
 * createOpenList - Sets status of each slot.  If slot.val = 0 then OPEN.
 *
 */
int createOpenList()
{
	int i,j;
	showGrid();
	for( i=0; i<GRID_SIZE; i++ )
		for( j=0; j<GRID_SIZE; j++ )
		{
			if (grid[i][j].val == 0 && grid[i][j].status != CLOSED)
			{
				grid[i][j].i = i;
				grid[i][j].j = j;
				grid[i][j].val = 0;
				grid[i][j].status = OPEN;
				countOpen++;
				//printf("opening: [%u][%u]\n",i,j);
			}
			else
			{
				if(grid[i][j].status != OPEN)
					grid[i][j].status = CLOSED;
			}
		}
		
	return countOpen;
}

/*
 * selectPossible( struct SLOT *slot) 
 *
 */
int selectPossible( struct SLOT *slot)
{
	int i,j,k,l,p,z;
	int valuePool[GRID_SIZE];

	//reset possibles
	for(i=0; i < GRID_SIZE; i++)
	{
		slot->possibles[i] = i+1;
		valuePool[i] = i;
	}

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
	p=0;
	k = 0;
	for(i=0; i < GRID_SIZE; i++)
	{
		if(slot->possibles[i] != 0)
		{
			p++; //number of possible answers..
			valuePool[k] = i+1; //answers to choose from
			k++;
		}
	}
	
	//Note: This may return 0 is there are no possible values
	z = randInt(0,k);
	return valuePool[z];

}

//setRandomState( slot ) - sets the slot value to a random value.
//	The restrain parameter forces the new value to be legal
void setRandomState(SLOT *slot, int restrain)
{
	srandom(time(0));
	if(restrain) //something wrong with this...
	{
		//Select a random value that is plausible 
		//given the puzzle's rules...
		slot->val = selectPossible(slot);
		
		//slot->val = randInt(1,9);
		//printf("%d\n", slot->val);
	}else{
		slot->val = randInt(1,9);
	}
}

//randomOpenSlot( openCount ) - returns a pointer to a random
//open slot
SLOT* randomOpenSlot(int openCount)
{
	int i,j,k, openSlotIndex;
	
	openSlotIndex = randInt(1,openCount); //0 to # of open slots
	
	//Loop over the entire puzzle since a linked list
	//was not setup for the open slots.
	k = 0;
	for( i=0; i<GRID_SIZE; i++ )
		for( j=0; j<GRID_SIZE; j++ )
			if(grid[i][j].status == OPEN)
			{
				k++;
				if(k == openSlotIndex)
				{
					//printf("openSlotIndex = %d\tk = %d\n",openSlotIndex, k);
					return &grid[i][j];
				}
			}
	printf("openCount = %d\t openSlotIndex = %d\tk = %d\n",openCount,openSlotIndex, k);
	return NULL;
}

//calculateSystemEnergy() - calculates the energy of the current system
double calculateSystemEnergy()
{
        int i, j, k, l, energy, v[GRID_SIZE];
		
		//Set the energy to max
		energy = 27;

		//Decrement the energy level for every correct row
		for( i=0; i<GRID_SIZE; i++ ) // check rows
		{
			for( j=0; j<GRID_SIZE; j++ )
					v[j] = grid[i][j].val;
			if ( isValid( v ) )
					energy--;
		}

		//Decrement the energy level for every correct col
		for( j=0; j<GRID_SIZE; j++ ) // check columns
		{
			for( i=0; i<GRID_SIZE; i++ )
					v[i] = grid[i][j].val;
			if ( isValid( v ) )
					energy--;
		}

		//Decrement the energy level for every correct sub-box
		for( i=0; i<BOX_SIZE; i++ ) // check boxes
			for( j=0; j<BOX_SIZE; j++ )
			{
				for( k=0; k<BOX_SIZE; k++ )
					for( l=0; l<BOX_SIZE; l++ )
						v[k*BOX_SIZE+l] = grid[i*BOX_SIZE+k][j*BOX_SIZE+l].val;
					if ( isValid( v ) )
							energy--;
			}

     return energy;
}

/*
 *     Stochastic Simulated Annealing
 */

int findSolution_SAnnealing(double T, double step)
{
	
	/*
	The Algorithm:
	1. Start w/ randomized states and a high temparature value (T).
	2. While no solution or T !converged
	3.		Choose a random node, n_i
	4.		Calculate system energy of current state, E_a
	5.		Change state of n_i
	6.		Recalculate new system energy in the new state, E_b
	7.		If E_b < E_a accept the state change.
	8.		If E_b > E_a accept the state change with a probablility:
							e^(deltaE_ab / T) , deltaE_ab = E_b - E_a
	9.		If every node has been "polled" more than once, stop
	10.		Else, goto 3.
	11.		Lower T.
	12. End While
	*/

	int i, j, k, nodeStateAtE_a;
	int check, openCount;
	double E_a, E_b, deltaE, converge;
	double boltzmannFactor;
	SLOT *node_i;

	//Setup Simulated Annealing Variables..
	converge = 0.0;
	if(createList == 0)
		openCount = createOpenList();
	createList = 1;
	printf("openCount = %d\n", openCount);
	
	//1. Randomize the state of open nodes
	for( i=0; i<GRID_SIZE; i++ )
		for( j=0; j<GRID_SIZE; j++ )
			if ( grid[i][j].status == OPEN )
				setRandomState(&grid[i][j], 0);
				
	//2. Begin polling OPEN nodes
	while(T > converge)
	{
		//Every node should be polled several times
		//...supposedly: (N/2)^k, k=3+
		for( i=0; i<81*3; i++ )//9^3=729
		{
			//Select a random open slot (node)
			node_i = randomOpenSlot(openCount);
			if(node_i != NULL)
			{
				//Calculate system energy of the current state, E_a
				E_a = calculateSystemEnergy();
				
				//This might be "cheating" since its an observational practice
				//that is not defined by the algorithm
				if(E_a == 0)
				{
					printf("E_a = %f\n", E_a);
					return 1;
				}
				
				//Save node's current state:
				nodeStateAtE_a = node_i->val;
				
				//Change the random node's state
				setRandomState(node_i,1);
				//printf("new value = %d\n", node_i->val);
				
				//Calculate system energy of the new state, E_b
				E_b = calculateSystemEnergy();
				if(E_b == 0)
				{
					printf("E_b = %f\n", E_b);
					return 1;
				}
				
				//Show me the energies
				//printf("E_b = %f\t E_a = %f\t \n", E_b, E_a);
				
				if(E_b <= E_a)
				{
					//accept the state change
				}else{
					//Only accept a higher energy state with
					//probability equal to:
					deltaE = E_b - E_a;
					boltzmannFactor = exp( (-1.0)*(deltaE/T) );
					//printf("bf = %f\t, dE = %f\n",boltzmannFactor, deltaE);
					
					//The Boltzmann Factor should be a percentage
					if(boltzmannFactor > randInt(0,1))
					//if(boltzmannFactor > randReal(0,1))
					{
						//accept the change
						//printf("boltzmann accepts.\n");
					}else{
						//change it back!
						//printf("boltzmann denied\n");
						node_i->val = nodeStateAtE_a;
					}
				}
			}else{printf("something went wrong\n");}
			
		}//end polling
		//printf("end polling\n");
	
//writeGrid(stdout);
		//Check for an answer
		check = isSolved();
		if ( check == 1 )
			return 1;
		else if ( check == 0 )
			T -= step;
			
		
	}
			

	return( 0 ); //didn't work.
	//Maybe I should set this up to increase T and start again?
}

int solvePuzzle()
{
	double T, step;
	T = 1000.0; 
	step = 0.1;
	
	//The trick with STOCHASTIC simulated annealing is to
	//accurately select the initial temperature and the 
	//rate at which the temperature is decreased.
	while( T < 10000000 )
	{
		if( findSolution_SAnnealing(T, step) )
		{
			printf("T = %f\n", T);
			return 1;
		}
		else
		{
			T += T;
			//step -= 0.1;
		}
	}
	
	//return( findSolution_DFS() );
	//return( findSolution_Astar() );
	
	return 0;
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

double randReal( double low, double high)
{
	rand()*(low-high) + low;
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
		printf( "Check Count: %ld\n", checkCount );
	}

	if ( bHTML )
		writeHTMLPuzzle( fopen("AnswerGrid.html", "w+") ); //to the command prompt??
	else
		writeGrid( stdout );

	return( 0 );
}

//main() - FOR TESTING WITH SAMPLE GRIDS:
/*
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


	return 0;

*/


