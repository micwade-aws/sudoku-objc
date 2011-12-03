#include <stdio.h>

void usage()
{
	printf("Usage of conversion program:\n");
	printf("./a.out <file.dat> <puzzle size> <char indicating blank>\n");
}

int main(int argc, char* argv[])
{
	int i,j, pSize, val;
	char *filename, *blank, temp;
	FILE *fp;

	//Check command line
	if(argc != 4){usage();return 1;}
	
	//Parse command line
	filename = argv[1];
	pSize = atoi(argv[2]);
	blank = argv[3];
	
	//Open the data file
	fp = fopen(filename,"r");

	//Reprint the grid to stdout.
	for(j=pSize; j>0; j--)
	{
		for(i=1; i<=pSize; i++)
		{
			//Get the next value
			temp = fgetc(fp);
			val = (temp == *blank) ? 0 : atoi(&temp);

			//Print to stdout: x y z
			printf("%d %d %d\n",i,j,val);

		}
		printf("\n");
	}

	//Close the file
	fclose(fp);
}
