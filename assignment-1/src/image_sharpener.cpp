#include <iostream>
#include "libppm.h"
#include <cstdint>

using namespace std;

struct image_t* return_padded_image(struct image_t *input_image){ // returns padding of 1px overall
	struct image_t* padded_image;
	int padded_height = input_image->height + 2; //new padded height
	int padded_width = input_image->width + 2;  //new padded width
	uint8_t*** original_image_matrix = input_image->image_pixels; //extracting original image matrix
	uint8_t*** padded_image_matrix = new uint8_t** [padded_height]; //allocating 3d matrix
	for(int i=0; i< (padded_height); i++){
		padded_image_matrix[i] = new uint8_t* [padded_width]; //allocating 2d matrix
		for(int j=0; j<((input_image->width)+2); j++){
			padded_image_matrix[i][j] = new uint8_t[3]; // allocating 1d RGB array
		}
	}
	for(int i=0; i<padded_height; i++){
		for(int j=0; j<padded_width; j++){
			if(i==0 || i==padded_height-1){ //if first or last row make it dark
				padded_image_matrix[i][j][0] = 255;
				padded_image_matrix[i][j][1] = 0;
				padded_image_matrix[i][j][2] = 0;
			}
			else if(j==0 || j== padded_width-1){ //else-if first or last column make it dark
				padded_image_matrix[i][j][0] = 255;
				padded_image_matrix[i][j][1] = 0;
				padded_image_matrix[i][j][2] = 0;
			}else{ //else just copy from original image
				padded_image_matrix[i][j][0] = original_image_matrix[i-1][j-1][0];
				padded_image_matrix[i][j][1] = original_image_matrix[i-1][j-1][1];
				padded_image_matrix[i][j][2] = original_image_matrix[i-1][j-1][2];
			}
		}
	}
	padded_image->image_pixels = padded_image_matrix;
	padded_image->height = padded_height;
	padded_image->width = padded_width;
	return padded_image;
}

struct image_t* S1_smoothen(struct image_t *input_image)
{
	// TODO
	// remember to allocate space for smoothened_image. See read_ppm_file() in libppm.c for some help.
	uint8_t*** image_pixel_matrix = input_image->image_pixels;
	cout << int(image_pixel_matrix[0][0][1]) << "\n";
	return input_image;
}

struct image_t* S2_find_details(struct image_t *input_image, struct image_t *smoothened_image)
{
	// TODO
	return 0;
}

struct image_t* S3_sharpen(struct image_t *input_image, struct image_t *details_image)
{
	// TODO
	return input_image; //TODO remove this line when adding your code
}

int main(int argc, char **argv)
{
	if(argc != 3)
	{
		cout << "usage: ./a.out <path-to-original-image> <path-to-transformed-image>\n\n";
		exit(0);
	}
	
	struct image_t *input_image = read_ppm_file(argv[1]);

	// cout << input_image->height;
	struct image_t *smoothened_image = S1_smoothen(input_image);
	struct image_t *padded_image = return_padded_image(input_image);
	
	// struct image_t *details_image = S2_find_details(input_image, smoothened_image);
	
	// struct image_t *sharpened_image = S3_sharpen(input_image, details_image);
	
	write_ppm_file(argv[2], padded_image);
	
	return 0;
}
