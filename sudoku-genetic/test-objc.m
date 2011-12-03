/* Michael J Wade
 * mike@iammyownorg.org
 * 
 * test.c
 * Use this to test ANYTHING...
 */
#import <stdio.h>
#import "objc/objc.h"
#import "objc/Object.h"
#import "icpak102.h"
#import "point.h"
#import "idarray.h"

#define IDARRAY 1

int main() {
	id a,b,c;
	int i,j,p;

#ifdef IDARRAY
	p = 10;
	a = [[IdArray new] capacity:(p)];
	for(i=0;i<p;i++)
		[a add:[Point x:0 y:i]];
	[a elementsPerform:@selector(printOn:) with:(void*)(stdout)];
	printf("\n--------------\n");
	[a sortBy:@selector(isAbove:)];
	printf("\n--------------\n");
	[a elementsPerform:@selector(printOn:) with:(void*)(stdout)];
	printf("\n");
	[a free];
#endif

	return 0;
}
