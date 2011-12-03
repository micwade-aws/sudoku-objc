/* Individual.m
 * Individual class implementation file.
 *
 * Michael J Wade 
 * mike@iammyownorg.org 
 * Copyright (c) 2007
 * 
 */

#import "Individual.h"

#define CHECKPT {printf("Checkpoint: %s, line %d\n", __FILE__,__LINE__);\
		fflush(stdout);}
#define STREAMS 256
#define OPEN   1
#define CLOSED 0

@implementation Individual

- copy {
	Individual *newIndividual;
	int i,j,c;
		
	//Create a new individual
	newIndividual = [Individual new];

	//Copy all instance values into the new individual
	newIndividual->ni 	= ni;
	newIndividual->nj 	= nj;
	newIndividual->sub_ni	= sub_ni;
	newIndividual->sub_nj 	= sub_nj;
	newIndividual->chromLen = chromLen;
	newIndividual->maxAllele= maxAllele; 
	newIndividual->maxFit	= maxFit;
	newIndividual->fitness 	= fitness;
	newIndividual->nOpen 	= nOpen;
	newIndividual->nClosed 	= nClosed;
	newIndividual->iOpen 	= iOpen;
	newIndividual->iClosed 	= iClosed;
	
	//Allocate the new chromosone
	newIndividual->chromosone = NULL;
	newIndividual->chromosone = (Gene*)objc_calloc(chromLen,sizeof(Gene));
	
	//Allocate the totals array.  This is used to keep track of
	//what has been used and how many times
	newIndividual->totals = (int*)calloc(maxAllele,sizeof(int));
	for( i=0; i < maxAllele; i++ )
		newIndividual->totals[i] = totals[i];
	
	//Copy in the new chromosone
	for( i=0; i < chromLen; i++ ){
		newIndividual->chromosone[i].status = chromosone[i].status;		
		newIndividual->chromosone[i].allele = chromosone[i].allele;		
		newIndividual->chromosone[i].xLoc   = chromosone[i].xLoc;
		newIndividual->chromosone[i].yLoc   = chromosone[i].yLoc;
		newIndividual->chromosone[i].Loc   = chromosone[i].Loc;
		//Allocate the new possibles array per gene
		newIndividual->chromosone[i].possibles = NULL;
		newIndividual->chromosone[i].possibles = (int*)objc_calloc(maxAllele,sizeof(int));
		for( j=0; j < maxAllele; j++ )
			newIndividual->chromosone[i].possibles[j] = chromosone[i].possibles[j];
	}

//	printf("Created new individual:\n");
	return newIndividual;
}

- free {
	int i;

	for( i=0; i < chromLen; i++ )
		free(chromosone[i].possibles);

	free(chromosone);
	//printf("freed the chromosone!");
	[super free];
	return nil;
}

- initialize:(IOD)anIOD m:(int)m n:(int)n subm:(int)sm subn:(int)sn maxFit:(int)mf{

	int i,j,c;

	//Setup the chromosone parameters
	ni = m;
	nj = n;
	sub_ni = sm;
	sub_nj = sn;
	chromLen = ni*nj;
	maxAllele = n; //maximum allele value can't exceed n
	maxFit = mf; //maximum fitness of a chromosone
	fitness = 0.0;
	nOpen = 0;
	nClosed = 0;
	
	//Allocate the new chromosone
	chromosone = NULL;
	chromosone = (Gene*)objc_calloc(chromLen,sizeof(Gene));
	
	//Allocate the totals array.  This is used to keep track of
	//what has been used and how many times
	totals = (int*)calloc(maxAllele,sizeof(int));

	//Read in the new chromosone
	for( j=(nj-1); j>=0; j-- )
	{
		for( i=0; i<ni; i++ )
		{
			c = fgetc( anIOD );
			if ( c == '.' ){
				chromosone[nj*i + j].allele = 0;
				chromosone[nj*i + j].status = OPEN;
				nOpen++;
	
				chromosone[nj*i + j].xLoc = i;
				chromosone[nj*i + j].yLoc = j;
				chromosone[nj*i + j].Loc = (nj*i+j);

				//Allocate the new possibles array per gene
				chromosone[nj*i + j].possibles = NULL;
				chromosone[nj*i + j].possibles = (int*)objc_calloc(maxAllele,sizeof(int));
			}
			else
			{
				c = c - '0';
				if ( (c < 1) || (c > maxAllele) ) 
				{
					i--;
				}else{
					chromosone[nj*i + j].allele = c;
					chromosone[nj*i + j].status = CLOSED;
					nClosed++;
					
					chromosone[nj*i + j].xLoc = i;
					chromosone[nj*i + j].yLoc = j;
					chromosone[nj*i + j].Loc = nj*i+j;

					//Allocate the new possibles array per gene
					chromosone[nj*i + j].possibles = NULL;
					chromosone[nj*i + j].possibles = (int*)objc_calloc(maxAllele,sizeof(int));

					//Increment the total used values array
					totals[c-1]++;
				}
			}
		}
		fgetc(anIOD);
	}

	//Update the counts.
	iOpen = [self open];
	iClosed = [self closed];

	return self;
}

- load:(IOD)anIOD m:(int)m n:(int)n subm:(int)sm subn:(int)sn maxFit:(int)mf{
	
	//Free the memory previously belonging to contents
	free(chromosone); 

	//Re-initialize the puzzle
	return [self initialize:(anIOD) m:(m) n:(n) subm:sm subn:sn maxFit:mf];
}

- gene:(int)loc allele:(int)val {
	//printf("gene: %d val:%d\n",loc,val);
	
	//Decrement total count from old value
	if(chromosone[loc].allele){
		totals[(chromosone[loc].allele - 1)]--;
	}
	
	//Set the new value
	chromosone[loc].allele = val;
	
	//If, the new value is not 0, increase the total count
	if(chromosone[loc].allele){
		totals[(chromosone[loc].allele - 1)]++;
	}

	return self;
} 

//The randomize message tells an individual to randomize the 
//open values on it's chromosone
- randomize {
	int i;
	
	for(i = 0; i < chromLen; i++){
		SelectStream(time(NULL));
		if(chromosone[i].status == OPEN){
			[self gene:i allele: Equilikely(1,maxAllele)];
			//printf("%d\n",chromosone[i].allele);
		}
	}
			
	return self;
}

/********************************************************************* 
 * PRE: none.
 * POST: Replaces the incorrect values with a 0.
 * NOTE: This is not the best way. Some values will be replaced 
 *	 even if they may have been correct.  However, it is not 
 *	 the worst method either.  The dependence is on the order in 
 *	 which the values are removed.
 *********************************************************************/
- correctChromosone {
	int i,j,h,k,m,n,x,y;
	int newTruth;
	Gene *a,*b;

	//Correct the rows
	for( j=0; j<nj; j++ )
		//Replace the values one-by-one if incorrect & open	
		for( i=	0; i<ni; i++ ){
			a = &chromosone[nj*i+j];
			//If this is open and not 0
			if(a->status == OPEN && a->allele != 0){
				//IS there another open slot with the same vale?
				for(x=0; x<ni; x++){
					b = &chromosone[nj*x+j];
					if(b->Loc != a->Loc){	
						if(a->allele == b->allele){
							//printf("replace (%d,%d) with (%d,%d)\n",a->xLoc,a->yLoc,b->xLoc,b->yLoc);
							[self gene:a->Loc allele:0];
							break;
						}
					}
				}
			}
		}

	//Correct the columns
	for( i=0; i<ni; i++ )
		//Replace the values one-by-one if incorrect & open	
		for( j=	0; j<nj; j++ ){
			a = &chromosone[nj*i+j];
			//If this is open and not 0
			if(a->status == OPEN && a->allele != 0){
				//IS there another open slot with the same vale?
				for(x=0; x<ni; x++){
					b = &chromosone[nj*i+x];
					if(b->Loc != a->Loc){	
						if(a->allele == b->allele){
							//printf("replace (%d,%d) with (%d,%d)\n",a->xLoc,a->yLoc,b->xLoc,b->yLoc);
							[self gene:a->Loc allele:0];
							break;
						}
					}
				}
			}
		}

	//Correct the boxes
	for( n=0; n<sub_nj; n++ )
		for( m=0; m<sub_ni; m++ )
		{
			for( j=0; j<sub_ni; j++ )
				for( i=0; i<sub_nj; i++ ){
					a = &chromosone[(nj*( (m*sub_ni)+i) ) 
							+ ( (n*sub_nj)+j )];
					//If this is open and not 0
					if(a->status == OPEN && a->allele != 0){
						//IS there another open slot with the same vale?
						for( h=0; h<sub_ni; h++ )
							for( k=0; k<sub_nj; k++ ){
								b = &chromosone[(nj*( (m*sub_ni)+k) ) 
										+ ( (n*sub_nj)+h )];
								if(b->Loc != a->Loc){	
									if(a->allele == b->allele){
										[self gene:a->Loc allele:0];
										break;
									}
								}
							}
					}
				}
		}

	//Update the new found "Truth(s)"
	newTruth = 0;
	for(i=0; i<chromLen; i++){
		a = &chromosone[i];
		if(a->status == OPEN && a->allele > 0){
			a->status = CLOSED;
			nClosed++;
			nOpen--;
			newTruth++;
		}
	}

	if(newTruth)
		[self randomize];
		
	
	return self;
}

/********************************************************************* 
 * PRE: none.
 * POST: Returns a fitness of a vector [0 - n]
 *********************************************************************/
- (signed int) evalVector:(int*)v
{
	int i, vFit, *a;

	vFit = 0;

	//Allocate a vector of 0s
	a = (int*)objc_calloc(maxAllele,sizeof(int));

	//For each V_i increment A_(V_i) by 1
	//as long as V_i != 0
	for( i=0; i<ni; i++ )
		if ( v[i] )
			a[v[i]-1]++;

	//A perfect score for v[n] is n.
	for(i = 0; i < ni; i++){
		if(a[i] > 0)
			vFit++;	
	}

	objc_free(a);

	return vFit;
}

/********************************************************************* 
 * PRE: none.
 * POST: Returns the total fitness of a chromosone.
 * NOTE: This is a Sudoku specific fitness evalutation.
 *********************************************************************/
- (int) evalFitness {
	int i,j,m,n,w,*v;

	//Allocate a vector that will be used to evaluate the 
	//genes of the chromosone.
	v = (int*)objc_malloc(maxAllele*sizeof(int));
	
	//Reset the fitness value to maximum.
	fitness = 0;

	for( j=0; j<nj; j++ ) // check rows
	{
		for( i=0; i<ni; i++ )
			v[i] = chromosone[nj*i + j].allele;
		
		//Decrease fitness accourdingly	
		fitness += [self evalVector:v];
		//printf("%d += %d\n",fitness,[self evalVector:v]);
	}
	
	for( i=0; i<ni; i++ ) // check columns
	{
		for( j=0; j<nj; j++ )
			v[j] = chromosone[nj*i + j].allele;
	
		//Decrease fitness accourdingly	
		fitness += [self evalVector:v];
	}
	
	for( n=0; n<sub_nj; n++ ) // check boxes
		for( m=0; m<sub_ni; m++ )
		{
			w = 0;
			for( j=0; j<sub_ni; j++ )
				for( i=0; i<sub_nj; i++ ){
					v[w] = 
					chromosone[(nj*( (m*sub_ni)+i) ) + 
						       ( (n*sub_nj)+j )].allele;
					w++;
				}
			//Decrease fitness accourdingly	
			fitness += [self evalVector:v];
		}

	objc_free(v);
	return fitness;
}


/********************************************************************* 
 * PRE: none.
 * POST: Returns [tPossible - Known] / tPossible
 * NOTE: This is a normalized certainty metric for gene determinism
 *********************************************************************/
- (double) certaintyOf:(Gene *)gene {

	int i,j,iOffset,jOffset,*a;
	double rules,overlap,c;

	//Precaution:  Regardless of what is around, any gene with a closed status
	//		has a certainty of 100% since it is a known truth.
	if(gene->status == CLOSED)
		return 1.0;//100%
	
	//Initialize a unit vector with zeros
	a = (int*)objc_calloc(maxAllele,sizeof(int));
	
	//(Re)Set possibles Array (used in A*)
	for(i=0; i < ni; i++)
		gene->possibles[i] = i+1;
	
	//eliminate possible row values
	j = gene->yLoc;
	for(i=0; i<maxAllele; i++){
		if( /*chromosone[nj*i + j].status == CLOSED &&*/ chromosone[nj*i + j].allele > 0){
			a[(chromosone[nj*i + j].allele)-1]++;
			//c--;
			gene->possibles[(chromosone[nj*i+j].allele - 1)] = 0;
		}
	}
		

	//eliminate possible col values
	i = gene->xLoc;
	for(j=0; j<maxAllele; j++){
		if( /*chromosone[nj*i + j].status == CLOSED &&*/ chromosone[nj*i + j].allele > 0){
			a[(chromosone[nj*i + j].allele)-1]++;
			//c--;
			gene->possibles[(chromosone[nj*i+j].allele - 1)] = 0;
		}
	}

	//eliminate box connections (this is tricky)
	/*BOXES:
	*	A | B | C
	*	D | E | F
	*	G | H | I
	*/
	
	//Compute the offset into each subregion.
	iOffset = gene->xLoc - (gene->xLoc % sub_ni);
	jOffset = gene->yLoc - (gene->yLoc % sub_nj);
	
	for( j=jOffset; j<(jOffset+sub_nj); j++ )
		for( i=iOffset; i<(iOffset+sub_ni); i++ ){
			if( /*chromosone[nj*i + j].status == CLOSED &&*/ chromosone[nj*i + j].allele > 0){
				a[(chromosone[nj*i + j].allele)-1]++;
				gene->possibles[(chromosone[nj*i+j].allele - 1)] = 0;
			}
		}

	//Compute the certainty value based on the remaining potential
	//values in a[].  This is a reduced form of the equation outlined
	//in the paper.
	c=0;
	for(i=0;i<maxAllele;i++){
		if(a[i] > 0)
		//if(gene->possibles[i] > 0)
			c++;
	}
	
	objc_free(a);
	
	//if(c == 0) c=maxAllele; //Is this a good idea? No.

	return  ((double)(maxAllele - c))/((double)maxAllele);
}

/*
 * removeCertainty(gene) - Returns the certainty that a gene's allele is absolute.
 *
 */
- removeUncertainty:(Gene *)gene below:(double)t {

	int i,j,iOffset,jOffset,*a;
	double rules,overlap,c;

	//Precaution:  Regardless of what is around, any gene with a closed status
	//		has a certainty of 100% since it is a known truth.
	if(gene->status == CLOSED)
		return self;//100%
	
	//Measure the certainty of this gene
	//	certainty = [summation of (Values per Vector) ] - Overlap
//	rules = 3;
//	overlap = 1;//Each rule/constraint will overlap the same gene
//	c = (((double)maxAllele)*rules)-overlap;	
	a = (int*)malloc(maxAllele*sizeof(int));

	//Initialize a unit vector
	for( i=0; i<ni; i++ )
		a[i] = 0;
	
	//eliminate possible row values
	j = gene->yLoc;
	for(i=0; i<maxAllele; i++)
		if( chromosone[nj*i + j].status == CLOSED )
			a[(chromosone[nj*i + j].allele)-1]++;
			//c--;

	//eliminate possible col values
	i = gene->xLoc;
	for(j=0; j<maxAllele; j++)
		if( chromosone[nj*i + j].status == CLOSED )
			a[(chromosone[nj*i + j].allele)-1]++;
			//c--;

	//eliminate box connections (this is tricky)
	/*BOXES:
	*	A | B | C
	*	D | E | F
	*	G | H | I
	*/
	
	//Compute the offset into each subregion.
	//Previous code used a bunch of if/else logic to accomplish
	//the same task!
	iOffset = gene->xLoc - (gene->xLoc % sub_ni);
	jOffset = gene->yLoc - (gene->yLoc % sub_nj);
	
	for( j=jOffset; j<(jOffset+sub_nj); j++ )
		for( i=iOffset; i<(iOffset+sub_ni); i++ )
			if( chromosone[nj*i + j].status == CLOSED )
				a[(chromosone[nj*i + j].allele)-1]++;

	//Compute the certainty value based on the remaining potential
	//values in a[].  This is a reduced form of the equation outlined
	//in the paper.
	c=0;
	for(i=0;i<maxAllele;i++){
		if(a[i] > 0)
			c++;
	}

	if(c <= t){
		for(i=0;i<maxAllele;i++){
			if(a[i] > 0)
				[self gene:(nj*(gene->xLoc) + gene->yLoc) allele:a[i] ];
		}
	}
	
	
	free(a);
	
	return self;
}

//This will return the Measurement of Uniqueness
//	In this measurement, the MU is the highest degree of uncertainty
- (double) MU {
	int i;
	double mu;
	
	mu = 0.0;
	//Remove Givens
	for(i=0; i<chromLen; i++)
		//if(mu < [self certaintyOf:(&chromosone[i])])
			mu += [self certaintyOf:(&chromosone[i])];

	return mu/(double)chromLen; //Normalized MU
}


/*
 * vague - will return 1 is the solution is improper and 0 if proper
 */
- (int) vague {

	//While there exists an x in P such that C(x) < C_k
	int i;
	double C_k, C_v;
	C_k = 2.0;  C_v = 8.0;
	int c = 1;
	
	while(c){
		//Cycle over chromosone and remove uncertainty
		for(i=0;i<chromLen;i++)
			[self removeUncertainty:&chromosone[i] below:C_k];

		//Cycle over chromosone and check for C(x) < C_k
		for(i=0;i<chromLen;i++){
			if([self certaintyOf:&chromosone[i] ] <= C_k)
				c = 1;
			else
				c = 0;
		}
	
	}
	
	//Cycle over chromosone and check for C(x) < C_k
	for(i=0;i<chromLen;i++){
		if([self certaintyOf:&chromosone[i] ] >= C_k)
			return 1;
	}

	//return 0 if non-vague/proper
	return 0;
}

- (BOOL) compareFitness: anIndividual {
    return (BOOL) (fitness < [anIndividual fitness]); 
}

- (BOOL) moveRemainsAt:(int)loc {
	int i;

	//Be certain that this is a valid location
	if(loc > chromLen || loc < 0)
		return 0;

	//Return TRUE if a possible exists
	for(i=0; i<maxAllele; i++)
		if(chromosone[loc].possibles[i])
			return 1;

	return 0;
}

- countTotals {
	int i;
	for(i=0;i<maxAllele;i++)
		totals[i] = 0;
	for(i=0;i<chromLen;i++)
		if(chromosone[i].allele)
			totals[ chromosone[i].allele - 1]++;

	return self;
}

-printTotals {
	int i;
	
	for(i=0;i<maxAllele;i++)
		printf(" %d ", totals[i]);
	
	return self;
}

//Assume that possibles has been computed...and that a possible does exist!
- (int) mostLikelyAt:(int)loc {
	int i,val;
	double p,tp;
	//printf("mostLikelyAt:%d\n",loc);
	

	//[self countTotals];	
	val = -1;//chromosone[loc].possibles[0];
	tp = 01.0; //was 10.0
	p = 01.0;//-99.0; //was 10.0
	//p = -99.0;//-99.0; //was 10.0
#if 0
	for(i=0;i<maxAllele;i++)
		printf(" %d ", chromosone[loc].possibles[i]);
	printf(" | ");
	for(i=0;i<maxAllele;i++)
		printf(" %d ", totals[i]);
#endif
	
	//Search for the possible value with ?? highest or lowest probability?
	//p = (((double)totals[0]) / ((double)maxAllele));
	for(i=0; i<maxAllele; i++){
		if(chromosone[loc].possibles[i]){
			tp = (((double)totals[i]) / ((double)maxAllele));
		if( tp < p /*tp > p || p == -99.0*/){// <
			p = tp;
			val = chromosone[loc].possibles[i];
		}}
	}

	//Eliminate this possible from future choices
	if(val > 0)
		chromosone[loc].possibles[val-1] = 0;

#if 0
	printf("\n");
	for(i=0;i<maxAllele;i++)
		printf(" %d ", chromosone[loc].possibles[i]);
	printf(" | ");
	for(i=0;i<maxAllele;i++)
		printf(" %d ", totals[i]);
#endif
	return val;
}

//The maxAllele is determined by the matrix size.
//TODO: rethink this and change it to be more dynamic.
//	perhaps make it a command line arg.
- (int) maxAllele {
	return maxAllele; 
}

- (int) chromLen {
	return chromLen;
}

- (int) rowLen {
	return ni; //Same thing.
}

- (int) alleleAt:(int)loc{
	return chromosone[loc].allele;
}

- (int) statusAt:(int)loc{
	return chromosone[loc].status;
}

//Display the puzzle's contents
- printOn:(IOD)anIOD {
	int i, j;
	double c;
	Gene *gene;
	
	//TODO: determine if the DOS shell has color codes
	//terminal color codes (UNIX):
	const char *const green = "\033[0;0;32m";
	const char *const red = "\033[0;0;31m";
	const char *const blue = "\033[0;0;34m";
	const char *const normal = "\033[0m";

	//Print Allele Values
	printf("Gene Alelles:\n");
	for( j=(nj-1); j>=0; j-- )
	{
		//fprintf(anIOD,"%i: ", i+1);
		for( i=0; i<ni; i++ )
		{
			gene = &chromosone[nj*i + j];
			if(chromosone[nj*i + j].allele == 0){
				fprintf(anIOD,green); //show blanks
				fprintf(anIOD,"%u", gene->allele); //show open
				//fprintf(anIOD,"(%d,%d) %u", gene->xLoc, gene->yLoc, gene->allele); //show open
			}else if(chromosone[nj*i + j].status == OPEN){
				fprintf(anIOD,red); //show open
				fprintf(anIOD,"%u",  chromosone[nj*i + j].allele); //show open
			}else{
				fprintf(anIOD,normal); //show closed
				fprintf(anIOD,"%u",  chromosone[nj*i + j].allele); //show open
			}

			
				fprintf(anIOD," ");
		}
		fprintf(anIOD,normal); //return to normal
		fprintf(anIOD,"\n");
	}
	//Print variables:
	fprintf(anIOD, "Size: %d X %d\tFitness: %d\tMU:%f\n",ni,nj,fitness,[self MU]);
	
	return self;
}

- printChromosoneOn:(IOD)anIOD {
	int i, j, n;
	double c;
	Gene *gene;
	
	//Print Allele Values
	printf("Gene Alelles:\n");
	for( j=(nj-1); j>=0; j-- )
	{
		//fprintf(anIOD,"%i: ", i+1);
		for( i=0; i<ni; i++ )
		{
			//Print the border
			if((i%sub_ni)==0 )
				//fprintf(anIOD," ");
				fprintf(anIOD,"|| ");
			
			//Print the gene
			if(chromosone[nj*i+j].allele < 80)
				fprintf(anIOD," ");
			fprintf(anIOD,"%c",  (chromosone[nj*i + j].allele)+'0'); //show open
		
			fprintf(anIOD," ");
		}
		
		//Print the border
		if((j%sub_nj)==0 ){
			fprintf(anIOD,"\n");
			for(n=0;n<(ni*3)+sub_nj*3;n++)
				fprintf(anIOD,"~");
			fprintf(anIOD,"\n");
		}
		
		fprintf(anIOD,"\n");
	}
	
	return self;
}
	
//Display the puzzle's contents
- printCertainty:(IOD)anIOD {
	int i, j;
	double c;
	
	//TODO: determine if the DOS shell has color codes
	//terminal color codes (UNIX):
	const char *const green = "\033[0;0;32m";
	const char *const red = "\033[0;0;31m";
	const char *const blue = "\033[0;0;34m";
	const char *const normal = "\033[0m";
	//Print the Certainty of each gene
	printf("\nCertainty of Allele Prediction:\n");
	//for( i=0; i<ni; i++ )
	for( j=(nj-1); j>=0; j-- )
	{
		//fprintf(anIOD,"%i: ", i+1);
	//	for( j=0; j<nj; j++ )
		for( i=0; i<ni; i++ )
		{
			c = [self certaintyOf:(&chromosone[nj*i + j]) ];
			
			if(c == 1.){ //100%
				fprintf(anIOD,green); 
				fprintf(anIOD,"%1.3f", c);
			}else if(c == 0.9){//90%, this is a given.
				fprintf(anIOD,blue); 
				fprintf(anIOD,"%1.3f", c);
			}else{
				fprintf(anIOD,red); //Not for certain..
				fprintf(anIOD,"%1.3f", c);
			}
				fprintf(anIOD," ");
		}
		fprintf(anIOD,normal); //return to normal
		fprintf(anIOD,"\n");
	}

	//Print variables:
	fprintf(anIOD, "Size: %d X %d\tFitness: %d\tMU:%f\n",ni,nj,fitness,[self MU]);
	
	return self;
}

- (double) phenotype {
	return phenotype;
}
- (int) fitness {
	return fitness;
}

- fitness:(int)newFitVal {
	fitness = newFitVal;
	return self;
}

- (int) open {
	int i;
	nOpen = 0;
	for(i=0;i<chromLen;i++)
		if(chromosone[i].status == OPEN)
			nOpen++;
	
	return nOpen;
}

- (int) closed {
	int i;
	nClosed = 0;
	for(i=0;i<chromLen;i++)
		if(chromosone[i].status == CLOSED)
			nClosed++;

	return nClosed;
}

- (int) initOpen { return iOpen; }
- (int) initClosed { return iClosed; }

@end
