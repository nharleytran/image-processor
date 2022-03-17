#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include "image.h"
#include "ppm.h"
#include "texture_synthesis.h"
#include <string.h>


int main(void)
{
	FILE *in = fopen("161.ppm","rb");
    
    const Image* img = ReadPPM(in);

    Image* synimg = NULL;
    synimg = AllocateImage( 256 , 256 );

    for( unsigned int i=0 ; i<(unsigned int)(img->width) ; i++ )
	{
        for( unsigned int j=0 ; j<(unsigned int)(img->height) ; j++ ){

        synimg->pixels[i*synimg->width + j].r = img->pixels[i*img->width + j].r;
        synimg->pixels[i*synimg->width + j].b = img->pixels[i*img->width + j].b;
        synimg->pixels[i*synimg->width + j].g = img->pixels[i*img->width + j].g;
        synimg->pixels[i*synimg->width + j].a = img->pixels[i*img->width + j].a;
		}        
    }

   

    // int size = *(&unset+1) - unset;
    //Cannot use sizeof for a pointer array

    int size = synimg->height * synimg->width - img->height * img->width;

    // printf("%d ",size);
    int i = 0; 
    while(i < 20)
    {
    int * unset = find_unset(img, synimg);
    TBSPixel * TBSPixellist = create_TBSPixels(synimg,img ,synimg->width, synimg->height,unset);

    for( int i=0 ; i< 131; i++ ){
       printf("x=%d y=%d ",(TBSPixellist+i)->idx.x,(TBSPixellist+i)->idx.y);
    }
    // for( int i=0 ; i< size; i++ ){
    //     printf("%d ",unset[i]);
    //  }

    for(int i =0; i < size; i++){
     synimg->pixels[unset[i]].r = 255;
    }

    for(int i =0; i < sizeof(TBSPixellist); i++){
     int index = ((TBSPixellist+i)->idx.x)+(((TBSPixellist+i)->idx.y)*synimg->width);
     synimg->pixels[index].r = 255;
     synimg->pixels[index].g = 255;
     synimg->pixels[index].b = 255;
     synimg->pixels[index].a = 255;
    }
    i++;
    }

    fclose(in);
    
     FILE *out = fopen("test3.ppm","wb");
     WritePPM(out,synimg);
     fclose(out);


}
