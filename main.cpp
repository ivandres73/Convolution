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
#define IMG_SIZX 50
#define IMG_SIZY 45
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
void writeRGBtoImg(CImg<short>&, array<short, N>&, array<short, N>&, array<short, N>&);

template <size_t N>
void printMatrix(const array<short, N>&, short, short);

int main() {
    CImg<short> image("heart.jpg");

    short *ptr = image.data();
    for (short i=0; i < IMG_SIZY*3; i++) {
        switch (i) {
            case 0: cout << "\t\033[1;31mRED MATRIX\033[0m\n"; break;
            case IMG_SIZY: cout << "\t\033[1;32mGREEN MATRIX\033[0m\n"; break;
            case IMG_SIZY*2: cout << "\t\033[1;34mBLUE MATRIX\033[0m\n"; break;
        }
        for (short v=0; v < IMG_SIZX; v++) {
            if (ptr[i*IMG_SIZX + v] <10)
                cout << "  " << ptr[i*IMG_SIZX + v] << ' ';
            else if (ptr[i*IMG_SIZX + v] <100)
                cout << " " << ptr[i*IMG_SIZX + v] << ' ';
            else
                cout << ptr[i*IMG_SIZX + v] << ' ';
        }
        cout << '\n';
    }

    array<short, RESULT_SIZE> red;
    array<short, RESULT_SIZE> gre;
    array<short, RESULT_SIZE> blu;
    applyFilter(ptr, kernel, red, gre, blu);

    CImg<short> new_image(result_x_size, result_y_size, 1, 3);
    
    writeRGBtoImg(new_image, red, gre, blu);

    image.display();
    new_image.display();
    new_image.save("hola.jpg");

    return 0;
}

template <size_t N>
void convolution(short *ptr, const array<short, KERNEL_SIZE>& kernel, array<short, N>& result) {
    ushort x_moves = IMG_SIZX - KERNEL_X + 1;
    ushort y_moves = IMG_SIZY - KERNEL_Y + 1;
    for (ushort i=0; i < y_moves; i++) {
        for (ushort v=0; v < x_moves; v++) {
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
    for (ushort i=0; i < y_dimension; i++) {
        for (ushort v=0; v < x_dimension; v++) {
            if (matrix[i*x_dimension + v] <10)
                cout << "  " << matrix[i*x_dimension + v] << ',';
            else if (matrix[i*x_dimension + v] <100)
                cout << " " << matrix[i*x_dimension + v] << ',';
            else
                cout << matrix[i*x_dimension + v] << ',';
        }
        cout << '\n';
    }
}

template <size_t N>
void applyFilter(short* ptr, const array<short, KERNEL_SIZE>& kernel, array<short, N>& red, array<short, N>& green, array<short, N>& blue) {
    cout << "\tnew red matrix:\n";
    convolution(ptr, kernel, red);
    printMatrix(red, result_x_size, result_y_size);
    ptr += IMAGE_CHANNEL_SIZE;
    cout << "\tnew green matrix:\n";
    convolution(ptr, kernel, green);
    printMatrix(green, result_x_size, result_y_size);
    ptr += IMAGE_CHANNEL_SIZE;
    cout << "\tnew blue matrix:\n";
    convolution(ptr, kernel, blue);
    printMatrix(blue, result_x_size, result_y_size);
}

template <size_t N>
void writeRGBtoImg(CImg<short>& new_image, array<short, N>& red, array<short, N>& gre, array<short, N>& blu) {
    for (register short i=0; i < result_y_size; i++) {
        short inicio = i * result_x_size;
        for (register short v=0; v < result_x_size; v++) {
            new_image[i*result_x_size + v] = red[inicio + v];
        }
    }
    for (register short i=result_y_size; i < result_y_size*2; i++) {
        short inicio = (i-result_y_size) * result_x_size;
        for (register short v=0; v < result_x_size; v++) {
            new_image[i*result_x_size + v] = gre[inicio + v];
        }
    }
    for (register short i=result_y_size*2; i < result_y_size*3; i++) {
        short inicio = (i-result_y_size*2) * result_x_size;
        for (register short v=0; v < result_x_size; v++) {
            new_image[i*result_x_size + v] = blu[inicio + v];
        }
    }
}