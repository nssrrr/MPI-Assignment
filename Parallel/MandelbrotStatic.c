#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define WIDTH 800
#define HEIGHT 800
#define MAX_ITER 1000

typedef struct {
    double real, imag;
} Complex;

int calculate_mandelbrot(Complex c) {
    int iter = 0;
    Complex z = { 0, 0 };
    
    while (iter < MAX_ITER && (z.real * z.real + z.imag * z.imag) < 4.0) {
        double temp_real = z.real * z.real - z.imag * z.imag + c.real;
        z.imag = 2.0 * z.real * z.imag + c.imag;
        z.real = temp_real;
        iter++;
    }
    
    return iter;
}

int main(int argc, char **argv) {
    double start_time, end_time, final_time;
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    start_time = MPI_Wtime();
    int rows_per_process = HEIGHT / size;
    int start_row = rank * rows_per_process;
    int end_row = start_row + rows_per_process;
    
    int mandelbrot[WIDTH * rows_per_process];
    
    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < WIDTH; j++) {
            double real = (j - WIDTH / 2.0) * 4.0 / WIDTH;
            double imag = (i - HEIGHT / 2.0) * 4.0 / HEIGHT;
            mandelbrot[(i - start_row) * WIDTH + j] = calculate_mandelbrot((Complex){real, imag});
        }
    }
    
    // Gather the results
    int *global_mandelbrot = NULL;
    if (rank == 0) {
        global_mandelbrot = (int *)malloc(WIDTH * HEIGHT * sizeof(int));
    }
    
    MPI_Gather(mandelbrot, WIDTH * rows_per_process, MPI_INT,
               global_mandelbrot, WIDTH * rows_per_process, MPI_INT, 0, MPI_COMM_WORLD);
    
    end_time = MPI_Wtime();
    // Save the Mandelbrot set as a PNG image
    if (rank == 0) {
        final_time = end_time - start_time;
        printf("final time: %f\n", final_time);
        unsigned char *image = (unsigned char *)malloc(WIDTH * HEIGHT * 3);
        for (int i = 0; i < HEIGHT; i++) {
            for (int j = 0; j < WIDTH; j++) {
                int index = i * WIDTH + j;
                int value = global_mandelbrot[index];
                image[index * 3] = (unsigned char)(value % 256);
                image[index * 3 + 1] = (unsigned char)(value % 256);
                image[index * 3 + 2] = (unsigned char)(value % 256);
            }
        }
        stbi_write_png("mandelbrot.png", WIDTH, HEIGHT, 3, image, WIDTH * 3);
        free(image);
        free(global_mandelbrot);
    }
    
    MPI_Finalize();
    return 0;
}
