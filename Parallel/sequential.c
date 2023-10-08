#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

#define WIDTH 800
#define HEIGHT 800
#define MAX_ITER 1000

typedef struct {
    double real;
    double imag;
} Complex;

int mandelbrot(Complex c) {
    int iter;
    Complex z = {0.0, 0.0};

    for (iter = 0; iter < MAX_ITER; iter++) {
        if (z.real * z.real + z.imag * z.imag > 4.0)
            return iter;
        
        double real_temp = z.real * z.real - z.imag * z.imag + c.real;
        double imag_temp = 2 * z.real * z.imag + c.imag;

        z.real = real_temp;
        z.imag = imag_temp;
    }

    return iter;
}

void create_mandelbrot_image(uint8_t *image) {
    double x, y;
    double x_scale = 3.5 / WIDTH;
    double y_scale = 2.0 / HEIGHT;

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            x = j * x_scale - 2.5;
            y = i * y_scale - 1.0;

            Complex c = {x, y};
            int iter = mandelbrot(c);
        }
    }
}

int main() {
    uint8_t *image = (uint8_t *)malloc(WIDTH * HEIGHT);
    if (!image) {
        printf("Memory allocation failed.\n");
        return 1;
    }
    clock_t start, end;
    double time;

    start = clock();
    create_mandelbrot_image(image);
    end = clock();
    time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("time: %fs\n", time);

    free(image);
    return 0;
}
