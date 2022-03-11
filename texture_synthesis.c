#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include "texture_synthesis.h"

// compares tbs pixels 
int CompareTBSPixels( const void *v1 , const void *v2 )
{
	const TBSPixel *tp1 = (const TBSPixel *)v1;
	const TBSPixel *tp2 = (const TBSPixel *)v2;
	int d = tp1->neighborCount - tp2->neighborCount;
	if( d ) return -d;
	d = tp1->idx.y - tp2->idx.y;
	if( d ) return d;
	d = tp1->idx.x - tp2->idx.x;
	if( d ) return d;
	return tp2->r - tp1->r;
}

// sorts tbs pixels, returns zero if succeeded
int SortTBSPixels( TBSPixel *tbsPixels , unsigned int sz )
{
	unsigned int *permutation = (unsigned int*)malloc( sizeof(unsigned int)*sz );
	if( !permutation )
	{
		fprintf( stderr , "[ERROR] Failed to allocate memory for permutation: %d\n" , sz );
		return 1;
		exit(1);
	}
	for( unsigned int i=0 ; i<sz ; i++ ) permutation[i] = i;
	for( unsigned int i=0 ; i<sz ; i++ )
	{
		unsigned int i1 = rand() % sz;
		unsigned int i2 = rand() % sz;
		unsigned int tmp = permutation[i1];
		permutation[i1] = permutation[i2];
		permutation[i2] = tmp;
	}

	for( unsigned int i=0 ; i<sz ; i++ ) tbsPixels[i].r = permutation[i];
	free( permutation );

	qsort( tbsPixels , sz , sizeof( TBSPixel ) , CompareTBSPixels );

	return 0;
}


int * find_unset(const Image *img, int width, int height) {
	
	int *unset_list = malloc((height * width - img->height * img->width) * sizeof(int));
	
	unsigned int j = 0;
	
	for (unsigned int i = 0; i < height * width; i++) {
		if (((img->pixels)+i).a == 0) {
			unset_list[j] = i;	
			j++;
		}

	return unset_list;

}

TBSPixel * create_TBSPixels(const Image *img, int width, int height, int *unset_list) {
    
	int i = 0;
	int j = 0;

	TBSPixel * TBSPixels = (int *) calloc(sizeof(TBSPixel));

	while (*(unset_list+i) != NULL) {

		Int pos = *(unset_list+i);
		int alpha_counter = 0;

		// case 1: on corner

		if (pos == 0) {
		
			if (img->pixels[pos + 1].a != 0) {
				alpha_counter++;
			}
			if (img->pixels[pos + width].a != 0) {
				alpha_counter++;
			}
			if (img->pixels[pos + width + 1].a != 0) {
				alpha_counter++;
			}

		}

		else if (pos == width * (height-1)) {

			if (img->pixels[pos + 1].a != 0) {
				alpha_counter++;
			}
			if (img->pixels[pos - width].a != 0) {
				alpha_counter++;
			}
			if (img->pixels[pos - width + 1].a != 0) {
				alpha_counter++;
			}

		}

		else if (pos == width - 1) {

			if (img->pixels[pos - 1].a != 0) {
				alpha_counter++;
			}
			if (img->pixels[pos + width].a != 0) {
				alpha_counter++;
			}
			if (img->pixels[pos + width - 1].a != 0) {
				alpha_counter++;
			}

		}

		else if (pos == (width * height) - 1) {

			if (img->pixels[pos - 1].a != 0) {
				alpha_counter++;
			}
			if (img->pixels[pos - width].a != 0) {
				alpha_counter++;
			}
			if (img->pixels[pos - width - 1].a != 0) {
				alpha_counter++;
			}

		}


		// case 2: on edge but not corner
		else if (pos < width) {

			if (img->pixels[pos - 1].a != 0) {
				alpha_counter++;
			}
			if (img->pixels[pos + 1].a != 0) {
				alpha_counter++;
			}
			if (img->pixels[pos + width - 1].a != 0) {
				alpha_counter++;
			}
			if (img->pixels[pos + width].a != 0) {
				alpha_counter++;
			}
			if (img->pixels[pos + width + 1].a != 0) {
				alpha_counter++;
			}

		}
	
		else if (pos > width * (height - 1)) {

			if (img->pixels[pos - 1].a != 0) {
				alpha_counter++;
			}
			if (img->pixels[pos + 1].a != 0) {
				alpha_counter++;
			}
			if (img->pixels[pos - width - 1].a != 0) {
				alpha_counter++;
			}
			if (img->pixels[pos - width].a != 0) {
				alpha_counter++;
			}
			if (img->pixels[pos - width + 1].a != 0) {
				alpha_counter++;
			}

		}

		else if (pos % width == 0) {

			if (img->pixels[pos - width].a != 0) {
				alpha_counter++;
			}
			if (img->pixels[pos + width].a != 0) {
				alpha_counter++;
			}
			if (img->pixels[pos - width + 1].a != 0) {
				alpha_counter++;
			}
			if (img->pixels[pos + 1].a != 0) {
				alpha_counter++;
			}
			if (img->pixels[pos + width + 1].a != 0) {
				alpha_counter++;
			}

		}

		else if ((pos % width) - (width - 1) == 0) {

			if (img->pixels[pos - width].a != 0) {
				alpha_counter++;
			}
			if (img->pixels[pos + width].a != 0) {
				alpha_counter++;
			}
			if (img->pixels[pos - width - 1].a != 0) {
				alpha_counter++;
			}
			if (img->pixels[pos - 1].a != 0) {
				alpha_counter++;
			}
			if (img->pixels[pos + width - 1].a != 0) {
				alpha_counter++;
			}

		}

		// case 3: all other pixels
		else {

			if (img->pixels[pos - 1].a != 0) {
				alpha_counter++;
			}
			if (img->pixels[pos + 1].a != 0) {
				alpha_counter++;
			}
			if (img->pixels[pos + width - 1].a != 0) {
				alpha_counter++;
			}
			if (img->pixels[pos + width].a != 0) {
				alpha_counter++;
			}
			if (img->pixels[pos + width + 1].a != 0) {
				alpha_counter++;
			}
			if (img->pixels[pos - width - 1].a != 0) {
				alpha_counter++;
			}
			if (img->pixels[pos - width].a != 0) {
				alpha_counter++;
			}
			if (img->pixels[pos - width + 1].a != 0) {
				alpha_counter++;
			}

		}

		// if alpha counter > 0, create TBS pixel
		if (alpha_counter > 0) {
		
			TBSPixel temp = { {pos % width, idx.y = pos / width} , alpha_counter, 0}; 
			*(TBSPixels + j) = temp;
			j++;
			realloc(TBSPixels, sizeof(TBSPixel));
		}

    	i++;
	}

	return TBSPixels;

}

Image *SynthesizeFromExemplar( const Image *exemplar , unsigned int outWidth , unsigned int outHeight , unsigned int windowRadius , bool verbose )
{
	Image *synthesized = NULL;

	// TODO: IMPLEMENT THIS FUNCTION

	return synthesized;
}
