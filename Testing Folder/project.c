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
    synimg = AllocateImage( 128 , 128 );

    for (unsigned int col = 0; col < img->width; col++) {
        for (unsigned int row = 0; row < img->height; row++) {
		
		PixelIndex idx = {col, row};
		SetPixel(synimg, row, col, GetPixel(img, idx));

        }
    }



    // int size = *(&unset+1) - unset;
    //Cannot use sizeof for a pointer array

    int size = synimg->height * synimg->width - img->height * img->width;

    // printf("%d ",size);
   
   
    int * unset = find_unset(img, synimg);
    TBSPixel * TBSPixellist = create_TBSPixels(synimg,img ,synimg->width, synimg->height,unset);

    for(int i = 0; i < 130 ; i++) {        
        printf("x=%d y=%d ",(TBSPixellist+i)->idx.x,(TBSPixellist+i)->idx.y);
    }
    // for( int i=0 ; i< size; i++ ){
    //     printf("%d ",unset[i]);
    //  }

    for(int i =0; i < size; i++){
     synimg->pixels[unset[i]].r = 255;
    }

    for(int i =0; i < 129; i++){
     int index = ((TBSPixellist+i)->idx.x)+(((TBSPixellist+i)->idx.y)*synimg->width);
     synimg->pixels[index].r = 255;
     synimg->pixels[index].g = 255;
     synimg->pixels[index].b = 255;
     synimg->pixels[index].a = 255;
    }
   

    fclose(in);
    
     FILE *out = fopen("test3.ppm","wb");
     WritePPM(out,synimg);
     fclose(out);


}
