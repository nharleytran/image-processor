#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include "image.h"
#include "ppm.h"
#include "texture_synthesis.h"
#include <string.h>


int main( int argc , char *argv[] )
{

	if (argc != 6) {
		printf("Incorrect number of command line arguments");
		return 1;
	}

	// Seed the random number generator so that the code produces the same results on the same input.
	srand(0);

	// Get the time at the start of execution
	clock_t start_clock = clock();

	// TODO: IMPLEMENT THIS FUNCTION
	FILE *in = fopen(argv[1],"rb");
    const Image* img = ReadPPM(in);
	int outWidth = atoi(argv[3]);
	int outHeight = atoi(argv[4]);
	int windowRadius= atoi(argv[5]);

	printf("Synthetising...\n");
	Image* synimg = SynthesizeFromExemplar( img, outWidth , outHeight , windowRadius);
	printf("Done.\n");

	fclose(in);
    FILE *out = fopen(argv[2],"wb");
    WritePPM(out,synimg);
    fclose(out);
	// Get the time at the end of the execution
	clock_t clock_difference = clock() - start_clock;

	// Convert the time difference into seconds and print
	printf( "Synthesized texture in %.2f(s)\n" , (double)clock_difference/CLOCKS_PER_SEC );
	return 0;
}

