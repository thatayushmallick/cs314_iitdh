#include <iostream>
#include "libppm.h"
#include <cstdint>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
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

void shared_memory(struct image_t* input_image, struct image_t* padded_image, struct image_t* &output_image, char* op, int i, int iter, const std::chrono::_V2::steady_clock::time_point start_clk) {
    // const char *name = "/my_shared_memory";
    // int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    // if (shm_fd == -1){
    //     perror("shm_open");
    //     exit(EXIT_FAILURE);
    // }
    // size_t size = 1024;
    // if (ftruncate(shm_fd, size) == -1){
    //     perror("ftruncate");
    //     exit(EXIT_FAILURE);
    // }
	// void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    // if (ptr == MAP_FAILED){
    //     perror("mmap");
    //     exit(EXIT_FAILURE);
    // }

    uint8_t smoothpixel[3];
    pid_t cdetailpid;
    int height = input_image->height;
    int width = input_image->width;

    // Create shared memory for smoothened pixels
    const char *smooth_name = "/smooth_shared_memory";
    int smooth_fd = shm_open(smooth_name, O_CREAT | O_RDWR, 0666);
    if (smooth_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    size_t smooth_size = sizeof(uint8_t) * 3 * height * width;
    if (ftruncate(smooth_fd, smooth_size) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    void *smooth_ptr = mmap(NULL, smooth_size, PROT_READ | PROT_WRITE, MAP_SHARED, smooth_fd, 0);
    if (smooth_ptr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    uint8_t (*smooth_shm)[3] = (uint8_t (*)[3])smooth_ptr;

    /** Forking to create p2 from p1 */
    cdetailpid = fork();
    if (cdetailpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cdetailpid > 0) {
        /** Parent process - Smoothening (Process p1) */
        while (i != iter - 1) {
            for (int row = 1; row <= height; row++) {
                for (int col = 1; col <= width; col++) {
                    int smooth_image[3];
                    for (int k = 0; k < 3; k++) {
                        smooth_image[k] = (padded_image->image_pixels[row - 1][col - 1][k] / 9 +
                                           padded_image->image_pixels[row - 1][col][k] / 9 +
                                           padded_image->image_pixels[row - 1][col + 1][k] / 9 +
                                           padded_image->image_pixels[row][col - 1][k] / 9 +
                                           padded_image->image_pixels[row][col][k] / 9 +
                                           padded_image->image_pixels[row][col + 1][k] / 9 +
                                           padded_image->image_pixels[row + 1][col - 1][k] / 9 +
                                           padded_image->image_pixels[row + 1][col][k] / 9 +
                                           padded_image->image_pixels[row + 1][col + 1][k] / 9);
                        if (smooth_image[k] > 255) {
                            smooth_image[k] = 255;
                        }
                        smooth_shm[(row - 1) * width + (col - 1)][k] = (uint8_t)smooth_image[k];
                    }
                }
            }
            i++;
        }

        wait(NULL);  // Wait for p2 to finish
        // Clean up shared memory
        munmap(smooth_ptr, smooth_size);
        shm_unlink(smooth_name);
    } else if (cdetailpid == 0) {
        /** Child process - Detailing (Process p2) */
        const char *detail_name = "/detail_shared_memory";
        int detail_fd = shm_open(detail_name, O_CREAT | O_RDWR, 0666);
        if (detail_fd == -1) {
            perror("shm_open");
            exit(EXIT_FAILURE);
        }

        size_t detail_size = sizeof(uint8_t) * 3 * height * width;
        if (ftruncate(detail_fd, detail_size) == -1) {
            perror("ftruncate");
            exit(EXIT_FAILURE);
        }

        void *detail_ptr = mmap(NULL, detail_size, PROT_READ | PROT_WRITE, MAP_SHARED, detail_fd, 0);
        if (detail_ptr == MAP_FAILED) {
            perror("mmap");
            exit(EXIT_FAILURE);
        }
        uint8_t (*detail_shm)[3] = (uint8_t (*)[3])detail_ptr;

        int rowdetail = 0;
        int coldetail = 0;
        while (i != iter - 1) {
            for (int row = 0; row < height; row++) {
                for (int col = 0; col < width; col++) {
                    int detail_pixel[3];
                    for (int k = 0; k < 3; k++) {
                        detail_pixel[k] = input_image->image_pixels[row][col][k] - smooth_shm[row * width + col][k];
                        if (detail_pixel[k] < 0) {
                            detail_pixel[k] = 0;
                        }
                        detail_shm[row * width + col][k] = (uint8_t)detail_pixel[k];
                    }
                }
            }
            rowdetail++;
            coldetail++;
            i++;
        }

        // Fork to create sharp process (p3)
        pid_t csharppid = fork();
        if (csharppid == 0) {
            /** Child process - Sharpening (Process p3) */
            int rowsharp = 0;
            int colsharp = 0;
            while (i != iter - 1) {
                for (int row = 0; row < height; row++) {
                    for (int col = 0; col < width; col++) {
                        int sharp_pixel[3];
                        for (int k = 0; k < 3; k++) {
                            sharp_pixel[k] = input_image->image_pixels[row][col][k] + detail_shm[row * width + col][k];
                            if (sharp_pixel[k] > 255) {
                                sharp_pixel[k] = 255;
                            }
                            output_image->image_pixels[row][col][k] = sharp_pixel[k];
                        }
                    }
                }
                rowsharp++;
                colsharp++;
                i++;
            }

            /** Output final result after all iterations */
            if (i == iter - 1) {
                const auto end_clk(chrono::steady_clock::now());
                const chrono::duration<double> time_read(end_clk - start_clk);
                std::cout << "TOTAL TIME FOR ALL ITERATION: " << (time_read.count()) << " SEC." << std::endl;
                write_ppm_file(op, output_image);
            }

            munmap(detail_ptr, detail_size);
            shm_unlink(detail_name);
            exit(EXIT_SUCCESS);
        }

        wait(NULL); // Wait for p3 to finish
        munmap(detail_ptr, detail_size);
        shm_unlink(detail_name);
        exit(EXIT_SUCCESS);
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
	shared_memory(input_image,padded_image,result_image,argv[2],0,iter,start_clk);

	return 0;
}
