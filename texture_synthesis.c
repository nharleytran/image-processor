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
		if (img->pixels[i].a == 0) {
			unset_list[j] = i;	
			j++;
		}
		else{
			unset_list = NULL;
		}

	return unset_list;

}

// Determines if the unset pixel has any set neighbors. 
// If so, creates a TBSPixel object for that pixel.
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
		
			TBSPixel temp = { {pos % width, pos / width} , alpha_counter, 0}; 
			*(TBSPixels + j) = temp;
			j++;
			realloc(TBSPixels, sizeof(TBSPixel));
		}

    	i++;
	}

	return TBSPixels;

}

Image *SynthesizeFromExemplar( const Image *exemplar , unsigned int outWidth , unsigned int outHeight , unsigned int windowRadius , bool flag )
{
	Image* synimg = NULL;
	flag = FALSE;
    synimg = AllocateImage( outWidth , outHeight);

	while (flag == FALSE)
	{
	  for( unsigned int i=0 ; i<(unsigned int)(img->width) ; i++ )
	{
        for( unsigned int j=0 ; j<(unsigned int)(img->height) ; j++ ){

        synimg->pixels[i*synimg->width + j].r = img->pixels[i*img->width + j].r;
        synimg->pixels[i*synimg->width + j].b = img->pixels[i*img->width + j].b;
        synimg->pixels[i*synimg->width + j].g = img->pixels[i*img->width + j].g;
        synimg->pixels[i*synimg->width + j].a = img->pixels[i*img->width + j].a;
	}        
    }
	 TBSPixel *TBSPixel_list = create_TBSPixels(exemplar, outWidth, outHeight, find_unset(synimg, Outwidth, Outheight);
	 int error = SortTBSPixels(TBSPixel_list,(sizeof(TBSPixel_list))/sizeof(TBSPixel));
	 assign_match(examplar, synimg, (*TBSPixel_list).idx.x,(*TBSPixel_list).idx.y , int windowsRadius) ;
	 flag = are_you_fill(synimg);9
	}
	
{

}

bool are_you_fill(Image *synimg, int * unset )
{
		if (unset== NULL) {
			bool flag = TRUE; 
	    else 
			flag = FALSE; 
			
}
void assign_match(const Image * img, Image * synimg, int colS, int rowS, int r) {

	float * sum_array = calloc(img->width * img->height * sizeof(float));
	for (int i = 0; i < img->width * img->height; i++) {

			*(sum_array + i) = compare_windows(img, colS, rowS, i % img->width, i / img->width, synimg->width, synimg->height, r);
	
	}

	float threshold = (1.1)*list_min(sum_array);

	int a = 0;

	for (int i = 0; i < img->width * img->height; i++) {
		if (*(sum_array+i) <= threshold) {
			a++;
		}
	}

	int counter = 0;
	int good_pixel_values[a] = {0};

	for (int i = 0; i < img->width * img->height; i++) {
		if (*(sum_array+i) <= threshold) {
			
			good_pixel_values[counter] = i;
			counter++;

		}
	}

	int chosen_position = RandomPick(a);

	synimg->pixels[colS+rowS*synimg->width].r = img->pixels[chosen_position].r;
	synimg->pixels[colS+rowS*synimg->width].g = img->pixels[chosen_position].g;
	synimg->pixels[colS+rowS*synimg->width].b = img->pixels[chosen_position].b;
	synimg->pixels[colS+rowS*synimg->width].a = img->pixels[chosen_position].a;
}

float compare_windows(const Image * img, int colS, int rowS, int colX, int rowX, int width, int height, int r) {

	float diff = 0.0;
	float sigma = ((2*r)+1)/6.4;

	for (int col = - r; col < r + 1; col++) {
		for (int row = - r; row <  r + 1; row++) {
	
			// check if pixel within TBSPixel window is within the image and set
			if (colS + col >= 0 && colS + col < width && rowS + row >= 0 && rowS + row < height && 
							img->pixels[(width * (rowS + row)) + (colS + col)].a != 0) {

				// check if pixel within exemplar pixel window is within image
				if (colX + col >= 0 && colX + col < width && rowX + row >= 0 &&  rowX + row < height) {
					
					float d = PixelSquaredDifference(img->pixels[(width * (rowS + row)) + (colS + col)], 
									img->pixels[((width * (rowS + row)) + (colS + col))]);
					float s = exp(-(col*col+row*row)/(2*sigma*sigma));
					diff += d * s;

				}
			}
		}
	}
	return sum;
}

//returns a window of size WindowSize around a given pixel 
int * GetNeigborhoodWindow(const Image *img, int w_width=5, int w_height=5, int width, int height,int pixel_index_x,int pixel_index_y) {
	
	int *neigborhood_list = malloc((w_height * w_width * sizeof(int));
	
	for(int j = 0 ; j< 5; j++)
	{
		for (int i = -2; i < 3; i++) {
			neigborhood_list[i+2] = ((img->pixels)+((pixel_index_x*pixel_index_y)+(j*width))+i).a;
		}

	}
	return neigborhood_list;

}

// randomly picks an element from a list
int RandomPick(int length) {

	return rand() % length;

}

// returns minimum value of a list
float list_min(float *a) {
	float * temp_a = a ; 
	size_t n = sizeof(temp_a) / sizeof(float);
 	int i, j, temp;
 		for (i=0;i< n-1;i++) {
			for (j=i+1;j< n;j++) {
				if (temp_a[i]> temp_a[j]) {
    				temp = temp_a[i];
					temp_a[i] = temp_a[j];
    				temp_a[j] = temp;
				}
			}
		}
	float min = temp_a[0];
	free(temp_a);
 	return min;
}

// generates a two-dimensional Gaussian in a window of given a 
// size centered in the center and with a given standard deviation 
/* float * Gaussian2D(int r, float sigma) {

	float * Gauss_window = calloc( ((2 * r) + 1) * ((2 * r) + 1) * sizeof(float) );

	int counter = 0;

	for (int row = -r; row < r + 1; row++) {
		for (int col = -r; col < r + 1; col++) {

			*(Gauss_window + counter) = exp(-(col*col+row*row)/(2*sigma*sigma));
			counter++;

		}
	}

	return Gauss_window;

}
*/