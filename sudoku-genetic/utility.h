/* Michael J Wade
 * mike@iammyownorg.org
 * 
 * random.h
 * Random functions needed for most probability based algorithms.
 */

#include <stdlib.h>

//Pseudo-random number on (0,1)
double randReal(int a, int b);

//Coin toss based on probability p
int flip(double p);

//Pseudo-random integer on [a,b]
int randInt(int a, int b);

