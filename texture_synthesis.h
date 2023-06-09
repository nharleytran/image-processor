#ifndef TEXTURE_SYNTHESIS_INCLUDED
#define TEXTURE_SYNTHESIS_INCLUDED
#include "image.h"

/** A struct storing information about a to-be-synthesized pixel*/
typedef struct
{
	/** The index of the pixel to be synthesized*/
	PixelIndex idx;
	
	/** The number of neighboring pixels in a set state*/
	unsigned int neighborCount;
	
	/** A member for storing a value used to resolve ties*/
	unsigned int r;
} TBSPixel;



/** A function that compares two TBSPixels and returns a negative number if the first should come earlier in the sort order and a positive number if it should come later*/
int CompareTBSPixels( const void *v1 , const void *v2 );

/** A function that sorts an array of TBSPixels*/
int SortTBSPixels( TBSPixel *tbsPixels , unsigned int sz );


// Find all unset pixels, which have alpha channel = 0
//int * find_unset(const Image *img, Image *synimg);

// Determines if the unset pixel has any set neighbors. 
// If so, creates a TBSPixel object for that pixel.
//TBSPixel * create_TBSPixels(Image *img, int width, int height, int *unset_list);
//TBSPixel *create_TBSPixels(Image *img,const Image *examplar ,int width, int height, int *unset_list);

//TBSPixel_List create_TBSPixels(Image *img, const Image *examplar ,int width, int height, int *unset_list);
// float compare_windows(const Image * img, int colS, int rowS, int colX, int rowX, int width, int height, int r);

// float list_min(float *a, int list_length);

// int RandomPick(int length);

// void assign_match(const Image * img, Image * synimg, int colS, int rowS, int r);

//bool are_you_fill(int * unset);



/** A function that extends the exemplar into an image with the specified dimensions, using the prescribed window radius -- the verbose argument is passed in to enable logging to the command prompt, if desired*/
Image *SynthesizeFromExemplar( const Image *exemplar , int outWidth , int outHeight , int windowRadius);
#endif // TEXTURE_SYNTHESIS_INCLUDED
