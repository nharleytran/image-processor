#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include "image.h"
#include "texture_synthesis.h"

// helper structure 
typedef struct
{
	TBSPixel* data; 
	unsigned int size;
} TBSPixel_List;

typedef struct
{
	int* data; 
	unsigned int size;
} int_array;
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


int_array find_unset(const Image *img, Image *synimg) {	
	int *unset_list = malloc((synimg->height * synimg->width - img->height * img->width) * sizeof(int));	
	int j = 0;
	
	for (unsigned int i = 0; i < synimg->height * synimg->width; i++) {
		if (synimg->pixels[i].a == 0) {
			unset_list[j] = i;	
			j++;
		}
	}

	printf("I am j = %d\n",j);
	int_array answer = {unset_list, j};
	return answer;
}



bool InBounds_wraper(Image* img, int row, int col) {
	PixelIndex idx = {col, row};
	return InBounds(img, idx);
}

Pixel* GetPixel_wraper(Image* img, int row, int col) {
	PixelIndex idx = {col, row};
	return GetPixel(img, idx);
}

// TODO: check for alpha channel == 0
// returns the number of set neighbors fo a pixel
int pixel_neighbors(Image *img, int row, int col) {
	return
	(InBounds_wraper(img, row - 1, col - 1) && GetPixel_wraper(img, row - 1, col - 1)->a != 0) +
	(InBounds_wraper(img, row - 1, col    ) && GetPixel_wraper(img, row - 1, col    )->a != 0) +
	(InBounds_wraper(img, row - 1, col + 1) && GetPixel_wraper(img, row - 1, col + 1)->a != 0) +
	(InBounds_wraper(img, row    , col - 1) && GetPixel_wraper(img, row    , col - 1)->a != 0) +
	(InBounds_wraper(img, row    , col + 1) && GetPixel_wraper(img, row    , col + 1)->a != 0) +
	(InBounds_wraper(img, row + 1, col - 1) && GetPixel_wraper(img, row + 1, col - 1)->a != 0) +
	(InBounds_wraper(img, row + 1, col    ) && GetPixel_wraper(img, row + 1, col    )->a != 0) +
	(InBounds_wraper(img, row + 1, col + 1) && GetPixel_wraper(img, row + 1, col + 1)->a != 0);
}

// Determines if the unset pixel has any set neighbors.
// If so, creates a TBSPixel object for that pixel.
TBSPixel_List create_TBSPixels(Image *img, const Image *examplar ,int width, int height, int *unset_list, const int unset_list_size) {
	TBSPixel_List TBSPixel_list = {(TBSPixel *) malloc(sizeof(TBSPixel) * (width + height)), 0};
	int j = 0;
	for (int i = 0; i < unset_list_size; i++) {
		int row = unset_list[i] / img->width;
		int col = unset_list[i] % img->width;
		int neighbors = pixel_neighbors(img, row, col);
		if (neighbors) {
			TBSPixel temp = { {col, row}, neighbors, 0}; 
			TBSPixel_list.data[j] = temp;
			j++;
		}
	}
	TBSPixel_list.size = j;
	return TBSPixel_list;
}

bool is_set(const Image *image, int row, int col) {
	return image->pixels[col + row * image->width].a != 0;
}

float compare_windows(Image* synimg, const Image* exemplar, int rowS, int colS, int rowX, int colX, int r) {
	float diff = 0;
	const float sigma = ((2.f * r) + 1.f) / 6.4f;
	for (int row = -r; row <= r; row++) {
		for (int col = -r; col <= r; col++) {
			if (InBounds_wraper(synimg, rowS + row, colS + col) && is_set(synimg, rowS + row, colS + col)) {
				if (!InBounds_wraper(exemplar, rowX + row, colX + col))
					return 0;
				float d = PixelSquaredDifference(
					*GetPixel_wraper(synimg,   rowS + row, colS + col),
					*GetPixel_wraper(exemplar, rowX + row, colX + col)
				);
				float s = exp(-(col * col + row * row) / (2.f * sigma * sigma));
				diff += d * s;
			}
		}
	}
	return diff;
}

/*
float compare_windows(const Image * img, int colS, int rowS, int colX, int rowX, int width, int height, int r) {

	float sum = 0.0;
	float sigma = ((2*r)+1)/6.4;

	for (int row = - r; row <  r + 1; row++) {
		for (int col = - r; col < r + 1; col++) {
	
			// check if pixel within TBSPixel window is within the image and set
			if (colS + col >= 0 && colS + col < width && rowS + row >= 0 && rowS + row < height && 
							img->pixels[(width * (rowS + row)) + (colS + col)].a != 0) {

				// check if pixel within exemplar pixel window is within image
				if (colX + col >= 0 && colX + col < width && rowX + row >= 0 &&  rowX + row < height&& 
							img->pixels[(width * (rowX + row)) + (colX + col)].a != 0) {
					
					float d = PixelSquaredDifference(img->pixels[(width * (rowX + row)) + (colX + col)], 
									img->pixels[((width * (rowS + row)) + (colS + col))]);
					float s = exp(-(col*col+row*row)/(2*sigma*sigma));
					sum += d * s;

				}
			}
		}
	}
	return sum;
}
*/

// returns minimum value of a list
float list_min(float *a, int list_length) {
	float min = a[0];
	for (int i = 0; i < list_length; i++)
		if (a[i] < min)
			min = a[i];
	return min;
}


// randomly picks an element from a list
int RandomPick(int length) {
	return rand() % length;
}

void assign_match(const Image * img, Image * synimg, int colS, int rowS, int r) {

	float * sum_array = calloc(img->width * img->height, sizeof(float));
	int i;
	for (i = 0; i < (int)img->width * (int)img->height; i++) {
		int rowX = i / img->width;
		int colX = i % img->width;
		*(sum_array + i) = compare_windows(synimg, img, rowS, colS, rowX, colX, r);
		//*(sum_array + i) = compare_windows(img, colS, rowS, i % img->width, i / img->width, synimg->width, synimg->height, r);
	
	}

	float threshold = (1.1)*list_min(sum_array, i);

	int a = 0;

	for (int i = 0; i < (int)img->width * (int)img->height; i++) {
		if (*(sum_array+i) <= threshold) {
			a++;
		}
	}

	int counter = 0;
	int good_pixel_values[a];

	for (int i = 0; i < (int)img->width * (int)img->height; i++) {
		
		if (*(sum_array+i) <= threshold) {
			
			good_pixel_values[counter] = i;
			counter++;

		}
	}

	int rand_good_pixel_idx = good_pixel_values[RandomPick(a)];
	
	SetPixel(synimg, rowS, colS, img->pixels + rand_good_pixel_idx);

}


Image *SynthesizeFromExemplar( const Image *exemplar , int outWidth , int outHeight , int windowRadius)
{
	Image* synimg = NULL;
	
    synimg = AllocateImage( outWidth , outHeight);

    for (unsigned int row = 0; row < exemplar->height; row++) {
		for (unsigned int col = 0; col < exemplar->width; col++) {
			PixelIndex idx = {col, row};
			SetPixel(synimg, row, col, GetConstPixel(exemplar, idx));

		}
	}

	int_array unset_list = find_unset(exemplar, synimg);

	while (unset_list.size > 0) {
	 TBSPixel_List TBSPixel_list = create_TBSPixels(synimg,exemplar, outWidth, outHeight, unset_list.data, unset_list.size);
	 //printf("tbspixel.\n");
	 

	 int error = SortTBSPixels(TBSPixel_list.data, TBSPixel_list.size);
	 //printf("sortpixels.\n");
	 if (error) {
		 return NULL;
	 }

	 assign_match(exemplar, synimg, (*TBSPixel_list.data).idx.x,(*TBSPixel_list.data).idx.y , windowRadius);
	 //printf("assign.\n");	
	 
	 free(TBSPixel_list.data); 
	 free(unset_list.data);
	 unset_list = find_unset(exemplar, synimg);
	}	
	
	return synimg;
}

