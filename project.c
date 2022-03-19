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

	if (argc != 6) { // error catch
		printf("Incorrect number of command line arguments\n");
		return 1;
	}

	// Seed the random number generator so that the code produces the same results on the same input.
	srand(0);

	// Get the time at the start of execution
	clock_t start_clock = clock();

	FILE *in = fopen(argv[1],"rb");
	if (in == NULL) { // error catch
		printf("Error opening input file\n");
		return 2;
	}

    Image* img = ReadPPM(in);
	if (img == NULL) { // error catch
		printf("Error reaing PPM file\n");
		return 3;
	}

	int outWidth = atoi(argv[3]);
	int outHeight = atoi(argv[4]);
	int windowRadius= atoi(argv[5]);

	printf("Synthesizing...\n");
	Image* synimg = SynthesizeFromExemplar( img, outWidth , outHeight , windowRadius);
	if (synimg == NULL) {
		printf("Error synthesizing image\n");
		return 5;
	}
	printf("Done.\n");

	fclose(in);
    FILE *out = fopen(argv[2],"wb");
	if (out == NULL) { // error catch
		printf("Error opening output file\n");
		return 4;
	}

    int error_check = WritePPM(out,synimg);
	if (error_check == -1) { // error catch
		printf("Error writing output ppm file\n");
		return 4;
	}
    fclose(out);

	// free images
	free(img);
	free(synimg);

	// Get the time at the end of the execution
	clock_t clock_difference = clock() - start_clock;

	// Convert the time difference into seconds and print
	printf( "Synthesized texture in %.2f(s)\n" , (double)clock_difference/CLOCKS_PER_SEC );
	return 0;
}

