#define cimg_use_jpeg //to read .jpg/.jpeg files

#include "CImg.h"
using cimg_library::CImg;
using cimg_library::CImgDisplay;

#include <iostream>
using std::cout;

#include <array>
using std::array;

// to compile: ~$ g++ main.cpp -L/usr/X11R6/lib -lm -lpthread -lX11 -ljpeg

#define IMG_SIZY 10
#define IMG_SIZX 10
#define KERNEL_X 3
#define KERNEL_Y 3
#define KERNEL_TOTAL 16
array<short, 9> kernel= {1, 2, 1,
                         2, 4, 2,
                         1, 2, 1 };

void convolution(short*, const array<short, 9>&, array<short, (IMG_SIZX - KERNEL_X + 1)*(IMG_SIZY - KERNEL_Y + 1)>&);
short kernel_calc(short *, short, short, const array<short, 9>&);

void printMatrix(const array<short, (IMG_SIZX - KERNEL_X + 1)*(IMG_SIZY - KERNEL_Y + 1)>&, short, short);

int main() {
    CImg<short> image("bw.jpg");
    //image.blur(2.5);
    //CImgDisplay main_disp(image,"Click a point");
    /*while (!main_disp.is_closed()) {
        main_disp.wait();
        if (main_disp.button() && main_disp.mouse_y()>=0) {
            const int y = main_disp.mouse_y();
        }
    }*/
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

    const short result_x_size = (IMG_SIZX - KERNEL_X) + 1;
    const short result_y_size = (IMG_SIZY - KERNEL_Y) + 1;
    array<short, result_x_size*result_y_size> red;
    array<short, result_x_size*result_y_size> gre;
    array<short, result_x_size*result_y_size> blu;
    cout << "new red matrix:\n";
    convolution(ptr, kernel, red);
    printMatrix(red, 8, 8);
    cout << "new green matrix:\n";
    convolution(ptr, kernel, gre);
    printMatrix(red, 8, 8);
    cout << "new blue matrix:\n";
    convolution(ptr, kernel, blu);
    printMatrix(red, 8, 8);

    CImg<short> new_image(8, 8, 1, 3);
    for (short i=0; i < 8; i++) {
        for (short v=0; v < 8; v++) {
            new_image[i*8 + v] = red[i*8 + v];
            //new_image(i*8 + v) = 10;
        }
    }
    for (short i=8; i < 16; i++) {
        short inicio = i-8;
        for (short v=0; v < 8; v++) {
            new_image[i*8 + v] = gre[inicio*8 + v];
            //new_image(i*8 + v) = 10;
        }
    }
    for (short i=16; i < 24; i++) {
        short inicio = i-16;
        for (short v=0; v < 8; v++) {
            new_image[i*8 + v] = blu[inicio*8 + v];
            //new_image(i*8 + v) = 10;
        }
    }
    //new_image.fill(red);

    image.display();
    cout << "valor de pixel=" << new_image(0);
    new_image.display();

    return 0;
}

void convolution(short *ptr, const array<short, 9>& kernel, array<short, (IMG_SIZX - KERNEL_X + 1)*(IMG_SIZY - KERNEL_Y + 1)>& result) {
    short x_moves = IMG_SIZX - KERNEL_X;
    short y_moves = IMG_SIZY - KERNEL_Y;
    //cout << "x_moves: " << x_moves << '\n';
    for (short i=0; i <= x_moves; i++) {
        for (short v=0; v <= y_moves; v++) {
            result[i*8 + v] = kernel_calc(ptr, i, v, kernel);
            //cout << kernel_calc(ptr, i, v, kernel) << ',';
        }
    }
}

short kernel_calc(short *ptr, short x_offset, short y_offset, const array<short, 9>& kernel) {
    short total_sum = 0;
    short row_sum;
    for (short i=0; i < 3; i++) {
        row_sum = 0;
        for (short v=0; v < 3; v++) {
            row_sum += kernel[v*3 + i] *  ptr[(i+x_offset)*10 + (v+y_offset)];//operacion es columna x fila
        }
        total_sum += row_sum;
    }
    return total_sum / KERNEL_TOTAL;
}

void printMatrix(const array<short, (IMG_SIZX - KERNEL_X + 1)*(IMG_SIZY - KERNEL_Y + 1)>& result, short x_dimension, short y_dimension) {
    for (short i=0; i < x_dimension; i++) {
        for (short v=0; v < y_dimension; v++) {
            cout << result[i*x_dimension + v] << ',';
        }
        cout << '\n';
    }
}