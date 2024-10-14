#include <iostream>
#include "libppm.h"
#include <cstdint>
#include <chrono>
#include <pthread.h>
#include <atomic>

using namespace std;

atomic_flag lock1 = ATOMIC_FLAG_INIT;
atomic_flag lock2 = ATOMIC_FLAG_INIT;
atomic_flag lock3 = ATOMIC_FLAG_INIT;
atomic_flag lock4 = ATOMIC_FLAG_INIT;

uint8_t* smoothened_output_pixel = new uint8_t[3];
uint8_t* detailed_output_pixel = new uint8_t[3];

int ITER;
int ITER_SMOOTH = 0;
int ITER_DETAIL = 0;
int ITER_SHARP = 0;

chrono::_V2::steady_clock::time_point START_TIME;
chrono::_V2::steady_clock::time_point END_TIME;

char* LOC;

void *S1_smoothen(void *thread_arg){
  image_t* input_image = (image_t*)thread_arg;
	uint8_t* smooth_px = new uint8_t[3];
	while(ITER_SMOOTH < ITER){
		for(int i=1; i<input_image->height-1; i++){
			for(int j=1; j<input_image->width-1; j++){
				for(int k=0; k<3; k++){
					int new_value = 0;
					for(int x=-1; x<=1; x++){ //thru row above to row below
						for(int y=-1; y<=1; y++){ // thru column left to column right
							new_value += (input_image->image_pixels[i+x][j+y][k])/9; //add up all 9 val.
						}
					}
					if(new_value>255){ // value must be constrained from 0,255
						new_value = 255;
					}
					smooth_px[k] = (uint8_t)new_value;
				}
				/**
				 * this is the critical section where we are 
				 * going to write the smoothened out pixel
				 */
				while(lock1.test_and_set()){}
				for(int z=0; z<3; z++){
					smoothened_output_pixel[z] = smooth_px[z];
				}
				lock2.clear();
			} 
		}
		ITER_SMOOTH+=1;
	}
  return NULL;
}

void *S2_details(void *thread_arg){
	image_t* input_image = (image_t*)thread_arg;
	uint8_t* smooth_pix = new uint8_t[3];
	uint8_t* detail_pix = new uint8_t[3];
	while(ITER_DETAIL<ITER){
		for(int i=0; i<input_image->height; i++){
			for(int j=0; j<input_image->width; j++){
				/**
				 * copying value is the critical section, once 
				 * it copies S2_details goes to non-critical section
				 */
				while(lock2.test_and_set()){}
				for(int z=0; z<3; z++){
					smooth_pix[z] = smoothened_output_pixel[z];
				}
				/** once copying is done let smooth pixel be globally
				 * written.
				 */
				lock1.clear();
				for(int k=0; k<3; k++){
					int value = 0;
					value = input_image->image_pixels[i][j][k] - smoothened_output_pixel[k]; 
					if(value<0){
						value = 0;
					}
					detail_pix[k] = uint8_t(value);
				}
				/**write the deatailed pixel globally*/
				while(lock3.test_and_set()){}
				for(int z=0; z<3; z++){
					detailed_output_pixel[z] = detail_pix[z];
				}
				/**let sharpen read the detailed pixel */
				lock4.clear();
			}
		}
		ITER_DETAIL+=1;
	}
	return NULL;
}

void* S3_sharpen(void *thread_arg){
	image_t* input_image = (image_t*) thread_arg;
	uint8_t* detail_pix = new uint8_t[3];
	uint8_t* sharp_pixel = new uint8_t[3];
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
	while(ITER_SHARP<ITER){
		for(int i=0; i<height; i++){
			for(int j=0; j<width; j++){
				/**critical section of copying the detailed image
				 * globally to this function.
				 */
				while(lock4.test_and_set()){}
				for(int z=0; z<3; z++){
					detail_pix[z] = detailed_output_pixel[z];
				}
				/**once copying is done, let S2 write the detail
				 * pixel globally
				 */
				lock3.clear();
				for(int k=0; k<3; k++){
					int value = 0;
					value = input_image->image_pixels[i][j][k] + detail_pix[k];
					if(value>255){
						value = 255;
					}
					sharpen_image_matrix[i][j][k] = uint8_t(value);
				}
			}
		}
		ITER_SHARP+=1;
	}
	const auto end_p(chrono::steady_clock::now());
	END_TIME = end_p;
	const chrono::duration<double> time_read(END_TIME - START_TIME);
	cout << "TOTAL TIME: " << (time_read.count()) << "SEC" << endl;
	image_t* sharpened_image = new image_t;
	sharpened_image->height = height;
	sharpened_image->width = width;
	sharpened_image->image_pixels = sharpen_image_matrix;
	cout << "writing to: " << LOC << " for " << ITER_SHARP << " TIMES." << endl;
	write_ppm_file(LOC, sharpened_image);
	lock1.clear();
	lock2.clear();
	lock3.clear();
	lock4.clear();
	return NULL;
}

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

int main(int argc, char **argv)
{
	if(argc != 3)
	{
		cout << "usage: ./a.out <path-to-original-image> <path-to-transformed-image>\n\n";
		exit(0);
	}
	LOC = argv[2];
	struct image_t *input_image = read_ppm_file(argv[1]);
	struct image_t *padded_image = return_padded_image(input_image);
  /**
   * let's make thread for parallel computation
   */
  ITER = 1000;
  pthread_t thread1, thread2, thread3;
  int iret1, iret2, iret3;

  lock1.clear();
  lock2.test_and_set(); /*do some wait*/
	lock3.clear();
	lock4.test_and_set(); /*do some wait ;)*/
	const auto start_p(chrono::steady_clock::now());
	START_TIME = start_p;
  iret1 = pthread_create(&thread1, NULL, S1_smoothen, padded_image);
  iret2 = pthread_create(&thread2, NULL, S2_details, input_image);
  iret3 = pthread_create(&thread3, NULL, S3_sharpen, input_image);
	
	pthread_join(thread1,NULL);
	pthread_join(thread2,NULL);
	pthread_join(thread3,NULL);

	exit(EXIT_SUCCESS);
	return 0;
}
