#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "image.h"

double PixelSquaredDifference( Pixel p1 , Pixel p2 )
{
	double d2 = 0;
	d2 += ((int)p1.r-(int)p2.r) * ((int)p1.r-(int)p2.r);
	d2 += ((int)p1.g-(int)p2.g) * ((int)p1.g-(int)p2.g);
	d2 += ((int)p1.b-(int)p2.b) * ((int)p1.b-(int)p2.b);
	return d2;
}

bool InBounds( const Image *image , PixelIndex idx )
{
	return idx.x<image->width && idx.y<image->height;
}

Pixel *GetPixel( Image *image , PixelIndex idx )
{
	if( !InBounds( image , idx ) )
	{
		fprintf( stderr , "[ERROR] GetPixel: Pixel image out of bounds: (%d,%d) not in [0,%d)x[0,%d)\n" , idx.x , idx.y , image->width , image->height );
		return NULL;
	}
	return image->pixels + idx.y*image->width + idx.x;
}

const Pixel *GetConstPixel( const Image *image , PixelIndex idx )
{
	if( !InBounds( image , idx ) )
	{
		fprintf( stderr , "[ERROR] GetConstPixel: Pixel image out of bounds: (%d,%d) not in [0,%d)x[0,%d)\n" , idx.x , idx.y , image->width , image->height );
		return NULL;
	}
	return image->pixels + idx.y*image->width + idx.x;
}

Image *AllocateImage( unsigned int width , unsigned int height )
{
  Image *image = malloc(sizeof(Image));
  if (*image == NULL) { // error check for unsuccessful memory allocation
    return NULL;
  }
  
  image.width = width;
  image.height = height;

  Pixel *pixels = malloc(width * height * 4 * sizeof(unsigned int)); 
  if (*pixels == NULL) { // error check for unsuccessful memory allocation
    return NULL;
  }
  
  image.pixels = pixels;

  return image;
}

void FreeImage( Image **image )
{
  free(image);
  *image = NULL;
}
