/* Michael J Wade
 * mike@iammyownorg.org
 * 
 * utility.c
 * Random functions needed for most probability based algorithms.
 */

#include "utility.h"

//Pseudo-random number on (0,1)
double randReal(int a, int b){
	return (a + (double)rand () * (b - a + 1) / RAND_MAX);
}

//Coin toss based on probability p
int flip(double p){
	srand(time(NULL));
	return rand() % 10;
}

//Pseudo-random integer on [a,b]
int randInt( int a, int b )
{
	return (a + (double)rand () * (b - a + 1) / RAND_MAX);
}
