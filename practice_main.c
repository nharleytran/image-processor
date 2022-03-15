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

    Image* synimg = NULL;
    synimg = AllocateImage( 128 , 128 );

    for( unsigned int i=0 ; i<(unsigned int)(img->width) ; i++ )
	{
        for( unsigned int j=0 ; j<(unsigned int)(img->height) ; j++ ){

        synimg->pixels[i*synimg->width + j].r = img->pixels[i*img->width + j].r;
        synimg->pixels[i*synimg->width + j].b = img->pixels[i*img->width + j].b;
        synimg->pixels[i*synimg->width + j].g = img->pixels[i*img->width + j].g;
        synimg->pixels[i*synimg->width + j].a = img->pixels[i*img->width + j].a;
	}        
    }

    int * unset = find_unset(synimg, synimg->width , synimg->height);
    unsigned int size = sizeof *unset/ sizeof unset[0];
    for( unsigned int i=0 ; i< size; i++ ){
        printf("%d ",unset[i]);
     }

    fclose(in);
    
     FILE *out = fopen("test1.ppm","wb");
     WritePPM(out,synimg);
     fclose(out);


}
