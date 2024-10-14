#include <iostream>
#include "libppm.h"
#include <cstdint>
#include <chrono>

using namespace std;

struct image_t* return_padded_image(struct image_t *input_image){ // returns padding of 1px overall
	int padded_height = (input_image->height) + 2; //new padded height
	int padded_width = (input_image->width) + 2;  //new padded width
	uint8_t*** original_image_matrix = input_image->image_pixels; //extracting original image matrix
	uint8_t*** padded_image_matrix = new uint8_t** [padded_height]; //allocating 3d matrix
	for(int i=0; i< (padded_height); i++){
		padded_image_matrix[i] = new uint8_t* [padded_width]; //allocating 2d matrix
		for(int j=0; j< padded_width; j++){
			padded_image_matrix[i][j] = new uint8_t[3]; // allocating 1d RGB array
		}
	}
	for(int i=0; i<padded_height; i++){
		for(int j=0; j<padded_width; j++){
			if(i==0 || i == padded_height-1 || j==0 || j == padded_width-1){ //if first or last row OR first/last colm make it dark
				padded_image_matrix[i][j][0] = 0;
				padded_image_matrix[i][j][1] = 0;
				padded_image_matrix[i][j][2] = 0;
			}else{ //else just copy from original image
				padded_image_matrix[i][j][0] = original_image_matrix[i-1][j-1][0];
				padded_image_matrix[i][j][1] = original_image_matrix[i-1][j-1][1];
				padded_image_matrix[i][j][2] = original_image_matrix[i-1][j-1][2];
			}
		}
	}
	struct image_t* padded_image = new struct image_t;
	padded_image->height = padded_height;
	padded_image->width = padded_width;
	padded_image->image_pixels = padded_image_matrix;
	return padded_image;
}

struct image_t* S1_smoothen(struct image_t *input_image)
{
	// remember to allocate space for smoothened_image. See read_ppm_file() in libppm.c for some help.
	uint8_t*** original_image_matrix = input_image->image_pixels; //extract matrix from original image
	int original_height = input_image->height; // slly extract height
	int original_width = input_image->width; //slly extract width
	uint8_t*** smooth_image_matrix = new uint8_t** [original_height-2]; // intialize 3-d matrix with h-2 rows
	for(int i=0; i<original_height-2; i++){
		smooth_image_matrix[i] = new uint8_t* [original_width-2]; // intitalise 2-d matrix with w-2 rows
		for(int j=0; j<original_width-2; j++){
			smooth_image_matrix[i][j] = new uint8_t[3]; //intialise 1-d matrix with 3 rows
		}
	}
	for(int i=1; i<original_height-1; i++){
		for(int j=1; j<original_width-1; j++){
			for(int k=0; k<3; k++){
				int new_value = 0;
				for(int x=-1; x<=1; x++){ //thru row above to row below
					for(int y=-1; y<=1; y++){ // thru column left to column right
						new_value += (original_image_matrix[i+x][j+y][k])/9; //add up all 9 val.
					}
				}
				if(new_value>255){ // value must be constrained from 0,255
					new_value = 255;
				}
				smooth_image_matrix[i-1][j-1][k] = uint8_t(new_value); //update new value must be type_casted to unsigned 8 bit format
			}
		}
	}
	struct image_t* smooth_image = new struct image_t;
	smooth_image->height = original_height-2;
	smooth_image->width = original_width-2;
	smooth_image->image_pixels = smooth_image_matrix;
	return smooth_image;
}

struct image_t* S2_find_details(struct image_t *input_image, struct image_t *smoothened_image)
{
	// let's extract some details from args
	uint8_t*** original_image_matrix = input_image->image_pixels;
	uint8_t*** smoothened_image_matrix = smoothened_image->image_pixels;
	int height = input_image->height;
	int width = input_image->width;
	// let's allocate space
	uint8_t*** detail_image_matrix = new uint8_t**[height];
	for(int i=0; i<height; i++){
		detail_image_matrix[i] = new uint8_t*[width];
		for(int j=0; j<width; j++){
			detail_image_matrix[i][j] = new uint8_t[3];
		}
	}
	// now for M(details) = M(original) - M(smoothened)
	for(int i=0;i<height; i++){
		for(int j=0;j<width; j++){
			for(int k=0; k<3; k++){
				int new_value = original_image_matrix[i][j][k] - smoothened_image_matrix[i][j][k];
				if(new_value<0){ // value must be constrained between 0,255
					new_value = 0;
				}
				detail_image_matrix[i][j][k] = uint8_t(new_value);
			}
		}
	}
	struct image_t* detail_image = new struct image_t;
	detail_image->height = height;
	detail_image->width = width;
	detail_image->image_pixels = detail_image_matrix;
	return detail_image;
}

struct image_t* S3_sharpen(struct image_t *input_image, struct image_t *details_image)
{
	// again collecting info from args	
	uint8_t*** original_image_matrix = input_image->image_pixels;
	uint8_t*** detail_image_matrix = details_image->image_pixels;
	int height = input_image->height;
	int width = input_image->width;
	// allocating space for sharpen-image-matrix
	uint8_t*** sharpen_image_matrix = new uint8_t** [height];
	for(int i=0; i<height; i++){
		sharpen_image_matrix[i] = new uint8_t*[width];
		for(int j=0; j<width; j++){
			sharpen_image_matrix[i][j] = new uint8_t[3];
		}
	}
	// M(sharpened-image) = M(detailed-image) + M(original-image)
	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			for(int k=0; k<3; k++){
				int value = (original_image_matrix[i][j][k]) + detail_image_matrix[i][j][k];
				if(value>255){ // value must be constrained between 0,255
					value = 255;
				}
				sharpen_image_matrix[i][j][k] = uint8_t(value);
			}
		}
	}
	struct image_t* sharpen_image = new struct image_t;
	sharpen_image->height = height;
	sharpen_image->width = width;
	sharpen_image->image_pixels = sharpen_image_matrix;
	return sharpen_image;
}

int main(int argc, char **argv)
{
	if(argc != 3)
	{
		cout << "usage: ./a.out <path-to-original-image> <path-to-transformed-image>\n\n";
		exit(0);
	}
	struct image_t *input_image = read_ppm_file(argv[1]);

	struct image_t *padded_image = return_padded_image(input_image);
	struct image_t *smoothened_image;
	struct image_t *details_image;
	struct image_t *sharpened_image;
	const auto start_p(chrono::steady_clock::now());
	int i=0;
	for(i=0; i<1000; i++){
	smoothened_image = S1_smoothen(padded_image);
	details_image = S2_find_details(input_image, smoothened_image);
	sharpened_image = S3_sharpen(input_image, details_image);
	}
	const auto end_p(chrono::steady_clock::now());
	const chrono::duration<double> time_read(end_p-start_p);
	cout << "writing image to: " << argv[2] << " for " << i << " times" << endl;
	cout << "TOTAL TIME: " << (time_read.count()) << " SEC\n";
	write_ppm_file(argv[2], sharpened_image);
	return 0;
}
