#include <iostream>
#include "libppm.h"
#include <cstdint>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

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

void start_pipe(struct image_t* input_image,struct image_t* padded_image, struct image_t* &output_image, char* op,int i, int iter, const std::chrono::_V2::steady_clock::time_point start_clk){
	/**this is process p1 here image gets smoothened out and the 
	 * value of each pixel is piped out to p2
	 */
	int detailpipefd[2];
	uint8_t smoothpixel[3];
	pid_t cdetailpid;
	int height = input_image->height;
	int width = input_image->width;
	/**pipe between p1 and p2 are created */
	if(pipe(detailpipefd)==-1){
		perror("pipe");
		exit(EXIT_FAILURE);
	}
	/**forking to create p2 from p1 */
	cdetailpid = fork();
	if(cdetailpid == -1){
		perror("fork");
		exit(EXIT_FAILURE);
	}
	if(cdetailpid>0){
		close(detailpipefd[0]); /*close the read end.*/
		while(i!=iter-1){
			for(int i=1; i<=height; i++){
				for(int j=1; j<=width; j++){
					int smooth_image[3];
					for(int k=0; k<3; k++){
						smooth_image[k] = (padded_image->image_pixels[i-1][j-1][k]/9 + padded_image->image_pixels[i-1][j][k]/9 + padded_image->image_pixels[i-1][j+1][k]/9
							+ padded_image->image_pixels[i][j-1][k]/9 + padded_image->image_pixels[i][j][k]/9 + padded_image->image_pixels[i][j+1][k]/9 + padded_image->image_pixels[i+1][j-1][k]/9
							+ padded_image->image_pixels[i+1][j][k]/9 + padded_image->image_pixels[i+1][j+1][k]/9);
							if(smooth_image[k]>255){
								smooth_image[k] = 255;
							}
							smoothpixel[k] = (uint8_t)smooth_image[k];
					}
					write(detailpipefd[1],smoothpixel,sizeof(uint8_t)*3);
				}
			}
			i++;
		}
		close(detailpipefd[1]); /** give EOF to detailpipefd */
		wait(NULL); /** waits for last pixel to get detailed */
	}else if(cdetailpid==0){
		close(detailpipefd[1]); /** block write end */
		int rowdetail = 0;
		int coldetail = 0;
		uint8_t readsmoothpixel[3];
		uint8_t writedetailpixel[3];
		int sharppipefd[2];
		if(pipe(sharppipefd)==-1){
			perror("pipe");
			exit(EXIT_FAILURE);
		}
		pid_t csharppid;
		csharppid = fork(); /*sharp pid */
		if(csharppid>0){
				while(read(detailpipefd[0],readsmoothpixel,sizeof(uint8_t)*3)>0){
					int detail_pixel[3];
					for(int i = 0; i<3; i++){
						detail_pixel[i] = input_image->image_pixels[rowdetail][coldetail][i] - readsmoothpixel[i];
						if(detail_pixel[i]<0){
							detail_pixel[i] = 0;
						}
						writedetailpixel[i] = (uint8_t)detail_pixel[i];
					}
					write(sharppipefd[1],writedetailpixel,sizeof(uint8_t)*3);
					coldetail++;
					if(coldetail==width){
						rowdetail = (rowdetail+1)%height;
						coldetail = 0;
					}
				}
			/**EOF from smooth has been recieved */
			close(detailpipefd[0]);
			close(sharppipefd[1]); /**send EOF to sharp*/
			wait(NULL); /**waits for the last pixel to get sharpened */
			exit(EXIT_SUCCESS);
		}else if(csharppid == 0){
			close(sharppipefd[1]); /**close write end */
			int rowsharp = 0;
			int colsharp = 0;
			uint8_t readdetailpixel[3];
			while(read(sharppipefd[0],readdetailpixel,sizeof(uint8_t)*3)>0){
				int sharp_pixel[3];
				for(int k=0; k<3; k++){
					sharp_pixel[k] = input_image->image_pixels[rowsharp][colsharp][k] + readdetailpixel[k];
					if(sharp_pixel[k]>255){
						sharp_pixel[k] = 255;
					}
					output_image->image_pixels[rowsharp][colsharp][k] = sharp_pixel[k];
				}
				colsharp++;
				if(colsharp==width){
					rowsharp = (rowsharp+1)%height;
					if(rowsharp==0){
						i++;
					}
					colsharp=0;
				}
			}
			/**EOF from detail is recieved*/
			close(sharppipefd[0]);
			/**if iteration are reached then only write */
			if(i==iter-1){
				const auto end_clk(chrono::steady_clock::now());
				const chrono::duration<double> time_read(end_clk-start_clk);
				cout << "TOTAL TIME FOR ALL ITERATION: " << (time_read.count()) << " SEC." << endl;
				write_ppm_file(op,output_image);
			}
			exit(EXIT_SUCCESS);
		}
	}
	return;
}

int main(int argc, char **argv)
{
	if(argc != 3)
	{
		cout << "usage: ./a.out <path-to-original-image> <path-to-transformed-image>\n\n";
		exit(0);
	}
	// now let's calculate time for each part.
	// const auto start_read(chrono::steady_clock::now());
	struct image_t *input_image = read_ppm_file(argv[1]);
	// const auto end_read(chrono::steady_clock::now());

	struct image_t *padded_image = return_padded_image(input_image);
	/**previously in assignment 1 we were passing the whole 
	 * image structure from one function to another, here we will be passing 
	 * a single pixel from one process to another using pipes..also last time we were initialising 
	 * a new matrix for each process, here we'll just have 3,the padded, original and the result image.
	 */
	/**allocating matrix space for result image. */
	struct image_t *result_image = new struct image_t;
	uint8_t*** result_image_matrix = new uint8_t**[input_image->height];
	for(int i=0; i<input_image->height; i++){
		result_image_matrix[i] = new uint8_t*[input_image->width];
		for(int j=0; j<input_image->width; j++){
			result_image_matrix[i][j] = new uint8_t[3];
		}
	}
	result_image->height = input_image->height;
	result_image->width = input_image->width;
	result_image->image_pixels = result_image_matrix;
	// const auto start_smooth(chrono::steady_clock::now());
	int iter = 1000;
	const auto start_clk(chrono::steady_clock::now());
	start_pipe(input_image,padded_image,result_image,argv[2],0,iter,start_clk);

	return 0;
}
