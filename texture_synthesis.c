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

float** exp_cache;

// initializes gaussian filter values
void initialize_exp_cache(int r) {
	exp_cache = malloc(sizeof(float*) * (2 * r + 1));
	for (int i = 0; i < 2 * r + 1; i++)
		exp_cache[i] = malloc(sizeof(float) * (2 * r + 1));
	
	const float sigma = ((2.f * r) + 1.f) / 6.4f;
	for (int row = -r; row <= r; row++)
		for(int col = -r; col <= r; col++)
			exp_cache[row + r][col + r] = exp(-(col * col + row * row) / (2.f * sigma * sigma));
}

void del_exp_cache(int r) {
	for (int i = 0; i < 2 * r + 1; i++)
		free(exp_cache[i]);
	free(exp_cache);
}

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
			unset_list[j] = (int) i;
			j++;
		}
	}

	//printf("I am j = %d\n",j);
	int_array answer = {unset_list, j};
	return answer;
}

bool InBounds_wraper(const Image* img, int row, int col) {
	PixelIndex idx = {col, row};
	return InBounds(img, idx);
}

Pixel* GetPixel_wraper(const Image* img, int row, int col) {
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
TBSPixel_List create_TBSPixels(Image *img, int width, int height, int *unset_list, const int unset_list_size) {
	TBSPixel_List TBSPixel_list = {(TBSPixel *) malloc(sizeof(TBSPixel) * (width + height)), 0};
	int j = 0;
	for (int i = 0; i < unset_list_size; i++) {
		int row = unset_list[i] / img->width;
		int col = unset_list[i] % img->width;
		int neighbors = pixel_neighbors(img, row, col);
		if (neighbors > 0) {
			TBSPixel temp = { {col, row}, neighbors, 0}; 
			TBSPixel_list.data[j] = temp;
			j++;
		}
	}
	TBSPixel_list.size = j;
	return TBSPixel_list;
}

// check if a pixel is set
bool is_set(const Image *image, int row, int col) {
	return image->pixels[col + row * image->width].a != 0;
}

// returns value of comparison accuracy between TBS pixel window and exemplar pixel window
// smaller value = more accurate
float compare_windows(Image* synimg, const Image* exemplar, int rowS, int colS, int rowX, int colX, int r) {
	//const float sigma = ((2.f * r) + 1.f) / 6.4f;
	float diff = 0;
	for (int row = -r; row <= r; row++) {
		for (int col = -r; col <= r; col++) {
			if (InBounds_wraper(synimg, rowS + row, colS + col) && is_set(synimg, rowS + row, colS + col)) {
				if (!InBounds_wraper(exemplar, rowX + row, colX + col))
					return -1;
				float d = PixelSquaredDifference(
					*GetPixel_wraper(synimg,   rowS + row, colS + col),
					*GetPixel_wraper(exemplar, rowX + row, colX + col)
				);
				//float s = exp(-(col * col + row * row) / (2.f * sigma * sigma));
				float s = exp_cache[row + r][col + r];
				diff += d * s;
			}
		}
	}
	return diff;
}

// returns value of comparison accuracy between TBS pixel window and exemplar pixel window
// smaller value = more accurate
float compare_windows_with_heuristic(Image* synimg, const Image* exemplar, int rowS, int colS, int rowX, int colX, int r, float h) {
	//const float sigma = ((2.f * r) + 1.f) / 6.4f;
	float diff = 0;
	for (int row = -r; row <= r; row++) {
		for (int col = -r; col <= r; col++) {
			if (InBounds_wraper(synimg, rowS + row, colS + col) && is_set(synimg, rowS + row, colS + col)) {
				if (!InBounds_wraper(exemplar, rowX + row, colX + col))
					return -1;
				float d = PixelSquaredDifference(
					*GetPixel_wraper(synimg,   rowS + row, colS + col),
					*GetPixel_wraper(exemplar, rowX + row, colX + col)
				);
				//float s = exp(-(col * col + row * row) / (2.f * sigma * sigma));
				float s = exp_cache[row + r][col + r];
				diff += d * s;
			}
		}
		if (diff > h)
			return -1;
	}
	return diff;
}

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

// chooses an exemplar pixel and assigns it to the chosen TBSPixel so that it becomes set
void assign_match(const Image * img, Image * synimg, int colS, int rowS, int r) {

	int * index_on_exemplar = calloc(img->width * img->height, sizeof(float));
	float * diff_array = calloc(img->width * img->height, sizeof(float));
	unsigned int diff_array_size = 0;

	int _i = 0;
	float min;

	for (_i = 0; _i < (int)img->width * (int)img->height && diff_array_size == 0; _i++) {
		int rowX = _i / img->width;
		int colX = _i % img->width;
		float diff = compare_windows(synimg, img, rowS, colS, rowX, colX, r);
		if (diff >= 0) {
			index_on_exemplar[diff_array_size] = _i;
			diff_array[diff_array_size] = diff;
			diff_array_size++;
			min = diff;
		}
	}

	for (int i = _i; i < (int)img->width * (int)img->height; i++) {
		int rowX = i / img->width;
		int colX = i % img->width;
		float diff = compare_windows_with_heuristic(synimg, img, rowS, colS, rowX, colX, r, min * 1.1);
		if (diff >= 0) {
			index_on_exemplar[diff_array_size] = i;
			diff_array[diff_array_size] = diff;
			diff_array_size++;
			if (diff < min)
				min = diff;
		}
	}

	float threshold = 1.1 * min;
	//float threshold = (1.1)*list_min(diff_array, diff_array_size);

	int *good_values = malloc(diff_array_size * sizeof(float));
	int good_values_size = 0;

	for (unsigned int i = 0; i < diff_array_size; i++) {
		if (diff_array[i] <= threshold) {
			good_values[good_values_size] = index_on_exemplar[i];
			good_values_size++;
		}
	}

	//printf("good values =%f\n",threshold);

	int rand_good_pixel_idx = good_values[RandomPick(good_values_size)];

	free(index_on_exemplar);
	free(diff_array);
	free(good_values);
	//printf("match: %d / %d\n", rand_good_pixel_idx, img->width * img->height);
	SetPixel(synimg, rowS, colS, img->pixels + rand_good_pixel_idx);
}


Image *SynthesizeFromExemplar( const Image *exemplar , int outWidth , int outHeight , int windowRadius)
{
	// allocate memory result output image
	Image* synimg = AllocateImage(outWidth, outHeight);

	// create gaussian filter
	initialize_exp_cache(windowRadius);

	// initializes pixel row and column positions for easy access later on
    for (unsigned int row = 0; row < exemplar->height; row++) {
		for (unsigned int col = 0; col < exemplar->width; col++) {
			PixelIndex idx = {col, row};
			SetPixel(synimg, row, col, GetConstPixel(exemplar, idx));
		}
	}

	// creates list of unset pixels before entering while loop
	int_array unset_list = find_unset(exemplar, synimg);

	while (unset_list.size > 0) { // while there are unset pixels in the output image, this loop sets pixels one at a time
	 	
		// find pixels from the unset list that are considered To-Be-Set pixels
		TBSPixel_List TBSPixel_list = create_TBSPixels(synimg, outWidth, outHeight, unset_list.data, unset_list.size);

		// error handling if SortTBSPixels fails
	 	int error = SortTBSPixels(TBSPixel_list.data, TBSPixel_list.size);
		if (error) {
			del_exp_cache(windowRadius);
			return NULL;
	 	}

		// sets a TBS pixel
	 	assign_match(exemplar, synimg, (*TBSPixel_list.data).idx.x, (*TBSPixel_list.data).idx.y , windowRadius);
	 
	 	// free dynamically allocated memory
		free(TBSPixel_list.data); 
		free(unset_list.data);

		// reevaluates unset pixel lit after each iteration of the while loop
		unset_list = find_unset(exemplar, synimg);

	}

	// free list of unset pixels
	free(unset_list.data);
	
	// frees gaussian filter data
	del_exp_cache(windowRadius);

	return synimg;
}
