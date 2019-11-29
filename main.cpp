#define cimg_use_jpeg //to read .jpg/.jpeg files

#include "CImg.h"
using cimg_library::CImg;
using cimg_library::CImgDisplay;

#include <iostream>
using std::cout;

#include <array>
using std::array;

// to compile: ~$ g++ main.cpp -L/usr/X11R6/lib -lm -lpthread -lX11 -ljpeg
// you can add '-Dcimg_use_png' flag to handle .png (RGBA) images

// IMPORTANT TO CHANGE THESE 
#define IMG_SIZY 10
#define IMG_SIZX 10
constexpr size_t IMAGE_CHANNEL_SIZE = IMG_SIZY * IMG_SIZX;

#define KERNEL_X 3
#define KERNEL_Y 3
#define KERNEL_TOTAL 16
array<short, 9> kernel= {1, 2, 1,
                         2, 4, 2,
                         1, 2, 1 };

constexpr size_t KERNEL_SIZE = KERNEL_X*KERNEL_Y;
constexpr size_t result_x_size = (IMG_SIZX - KERNEL_X + 1);
constexpr size_t result_y_size = (IMG_SIZY - KERNEL_Y + 1);
constexpr size_t RESULT_SIZE = result_x_size * result_y_size;

template <size_t N>
void convolution(short*, const array<short, KERNEL_SIZE>&, array<short, N>&);
short kernel_calc(short*, short, short, const array<short, KERNEL_SIZE>&);

template <size_t N>
void applyFilter(short*, const array<short, KERNEL_SIZE>&, array<short, N>&, array<short, N>&, array<short, N>&);

template <size_t N>
void printMatrix(const array<short, N>&, short, short);

int main() {
    CImg<short> image("square.jpg");

    short *ptr = image.data();
    short RGB = 0;
    for (short i=0; i < IMG_SIZX*3; i++) {
        switch (RGB) {
            case 0: cout << "RED MATRIX\n"; break;
            case 10: cout << "GREEN MATRIX\n"; break;
            case 20: cout << "BLUE MATRIX\n"; break;
        }
        for (short v=0; v < IMG_SIZY; v++) {
            cout << ptr[i*IMG_SIZX + v] << " ";
        }
        RGB++;
        cout << '\n';
    }

    array<short, RESULT_SIZE> red;
    array<short, RESULT_SIZE> gre;
    array<short, RESULT_SIZE> blu;
    applyFilter(ptr, kernel, red, gre, blu);

    CImg<short> new_image(8, 8, 1, 3);
    for (short i=0; i < 8; i++) {
        for (short v=0; v < 8; v++) {
            new_image[i*8 + v] = red[i*8 + v];
        }
    }
    for (short i=8; i < 16; i++) {
        short inicio = i-8;
        for (short v=0; v < 8; v++) {
            new_image[i*8 + v] = gre[inicio*8 + v];
        }
    }
    for (short i=16; i < 24; i++) {
        short inicio = i-16;
        for (short v=0; v < 8; v++) {
            new_image[i*8 + v] = blu[inicio*8 + v];
        }
    }

    image.display();
    new_image.display();
    new_image.save("hola.jpg");

    return 0;
}

template <size_t N>
void convolution(short *ptr, const array<short, KERNEL_SIZE>& kernel, array<short, N>& result) {
    ushort x_moves = IMG_SIZX - KERNEL_X + 1;
    ushort y_moves = IMG_SIZY - KERNEL_Y + 1;
    for (ushort i=0; i < x_moves; i++) {
        for (ushort v=0; v < y_moves; v++) {
            result[i*x_moves + v] = kernel_calc(ptr, i, v, kernel);
        }
    }
}

short kernel_calc(short *ptr, short x_offset, short y_offset, const array<short, KERNEL_SIZE>& kernel) {
    ushort total_sum = 0;
    ushort row_sum;
    for (ushort i=0; i < KERNEL_X; i++) {
        row_sum = 0;
        for (ushort v=0; v < KERNEL_Y; v++) {
            row_sum += kernel[v*3 + i] *  ptr[(i+x_offset)*IMG_SIZX + (v+y_offset)];//operacion es columna x fila
        }
        total_sum += row_sum;
    }
    return total_sum / KERNEL_TOTAL;
}

template <size_t N>
void printMatrix(const array<short, N>& matrix, short x_dimension, short y_dimension) {
    for (ushort i=0; i < x_dimension; i++) {
        for (ushort v=0; v < y_dimension; v++) {
            cout << matrix[i*x_dimension + v] << ',';
        }
        cout << '\n';
    }
}

template <size_t N>
void applyFilter(short* ptr, const array<short, KERNEL_SIZE>& kernel, array<short, N>& red, array<short, N>& green, array<short, N>& blue) {
    cout << "new red matrix:\n";
    convolution(ptr, kernel, red);
    printMatrix(red, result_x_size, result_y_size);
    ptr += IMAGE_CHANNEL_SIZE;
    cout << "new green matrix:\n";
    convolution(ptr, kernel, green);
    printMatrix(green, result_x_size, result_y_size);
    ptr += IMAGE_CHANNEL_SIZE;
    cout << "new blue matrix:\n";
    convolution(ptr, kernel, blue);
    printMatrix(blue, result_x_size, result_y_size);
}