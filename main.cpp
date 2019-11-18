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

void convolution(short*, const array<short, 9>&, array<short, IMG_SIZX*IMG_SIZY>&);
short kernel_calc(short *, short, short, const array<short, 9>&);

int main() {
    CImg<short> image("square.jpg");
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

    array<short, IMG_SIZX*IMG_SIZY> red;
    array<short, IMG_SIZX*IMG_SIZY> gre;
    array<short, IMG_SIZX*IMG_SIZY> blu;
    cout << "new red matrix:\n";
    convolution(ptr, kernel, red);
    cout << "new green matrix:\n";
    convolution(ptr, kernel, gre);
    cout << "new blue matrix:\n";
    convolution(ptr, kernel, blu);

    CImg<short> new_image(8, 8, 1, 3);
    for (short i=0; i < 8; i++) {
        for (short v=0; v < 8; v++) {
            if (RGB < 8)
                new_image[i*8 + v] = red[i*8 + v];
        }
    }
    //CImgDisplay main_disp2(new_image, "filter applied");
    new_image.display();
    bool wait;
    std::cin >> wait;

    return 0;
}

void convolution(short *ptr, const array<short, 9>& kernel, array<short, IMG_SIZX*IMG_SIZY>& result) {
    short x_moves = IMG_SIZX - KERNEL_X;
    short y_moves = IMG_SIZY - KERNEL_Y;
    cout << "x_moves: " << x_moves << '\n';
    for (short i=0; i <= x_moves; i++) {
        for (short v=0; v <= y_moves; v++) {
            cout << kernel_calc(ptr, i, v, kernel) << ',';
        }
        cout << '\n';
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