#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include "image.h"
#include "ppm.h"
#include "texture_synthesis.h"

int main(void)
{
	FILE *in = fopen("161.ppm","rb");
    
    const Image* img = ReadPPM(in);

    fclose(in);
    
    FILE *out = fopen("test.txt","wb");
    WritePPM(out,img);
    fclose(out);


}

