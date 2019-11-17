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

array<short, 9> kernel= {1, 2, 1,
                         2, 4, 2,
                         1, 2, 1 };

void convolution(short&, const array<short, 9>&, array<short, IMG_SIZX*IMG_SIZY>&);

int main() {
    CImg<short> image("square.jpg");
    //image.blur(2.5);
    CImgDisplay main_disp(image,"Click a point");
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

    return 0;
}

void convolution(short& ptr, const array<short, 9>& kernel, array<short, IMG_SIZX*IMG_SIZY>& result) {
    for (short i=0; i < IMG_SIZX; i++) {
        for (short v=0; v < IMG_SIZY; v++) {
            
        }
    }
}