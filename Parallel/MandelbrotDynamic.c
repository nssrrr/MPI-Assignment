#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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

int main() {
    double start_time, end_time, final_time;

    start_time = clock();
    int mandelbrot[WIDTH * HEIGHT];

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            double real = (j - WIDTH / 2.0) * 4.0 / WIDTH;
            double imag = (i - HEIGHT / 2.0) * 4.0 / HEIGHT;
            mandelbrot[i * WIDTH + j] = calculate_mandelbrot((Complex){real, imag});
        }
    }

    end_time = clock();
    final_time = (end_time - start_time) / CLOCKS_PER_SEC;
    printf("Final time: %fs\n", final_time);

    // Save the Mandelbrot set as a PNG image
    unsigned char *image = (unsigned char *)malloc(WIDTH * HEIGHT * 3);
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            int index = i * WIDTH + j;
            int value = mandelbrot[index];
            image[index * 3] = (unsigned char)(value % 256);
            image[index * 3 + 1] = (unsigned char)(value % 256);
            image[index * 3 + 2] = (unsigned char)(value % 256);
        }
    }

    stbi_write_png("mandelbrot_sequential.png", WIDTH, HEIGHT, 3, image, WIDTH * 3);
    free(image);

    return 0;
}
