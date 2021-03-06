#define cimg_use_jpeg //to read .jpg/.jpeg files

#include "CImg.h"
using cimg_library::CImg;
using cimg_library::CImgDisplay;

#include <iostream>
using std::cout;

#include <array>
using std::array;

#include "timer.h"
#include <omp.h>

// to compile: ~$ g++ main.cpp -L/usr/X11R6/lib -lm -lpthread -lX11 -ljpeg
// you can add '-Dcimg_use_png' flag to handle .png (RGBA) images

// IMPORTANT TO CHANGE THESE ACCORDING TO IMAGE SIZE
#define IMG_SIZX 1080
#define IMG_SIZY 720
constexpr size_t IMAGE_CHANNEL_SIZE = IMG_SIZY * IMG_SIZX;

#define KERNEL_X 3
#define KERNEL_Y 3
const array<short, 9> kernel= {0, -1, 0,
                               -1, 5, -1,
                               0, -1, 0 };

constexpr unsigned short GET_KERNEL_TOTAL() {
    unsigned short total = 0;
    for (auto i=0; i < kernel.size(); i++)
        total += kernel[i];
    return total;
}

const unsigned short KERNEL_TOTAL = GET_KERNEL_TOTAL();

constexpr size_t KERNEL_SIZE = KERNEL_X*KERNEL_Y;
constexpr size_t result_x_size = (IMG_SIZX - KERNEL_X + 1);
constexpr size_t result_y_size = (IMG_SIZY - KERNEL_Y + 1);
constexpr size_t RESULT_SIZE = result_x_size * result_y_size;

template <size_t N>
void convolution(const short*, const array<short, KERNEL_SIZE>&, array<short, N>&);
short kernel_calc(const short*, short, short, const array<short, KERNEL_SIZE>&);

template <size_t N>
void applyFilter(short*, const array<short, KERNEL_SIZE>&, array<short, N>&, array<short, N>&, array<short, N>&);

template <size_t N>
void writeRGBtoImg(CImg<short>&, array<short, N>&, array<short, N>&, array<short, N>&);

template <size_t N>
void printMatrix(const array<short, N>&, short, short);

// UTILITY GLOBAL VARIABLES
double total_time = 0;
unsigned short thread_count;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "usage is: " << argv[0] << " <file.jpg> <#-threads>\n";
        return 1;
    }

    CImg<short> image(argv[1]);
    thread_count = std::stoi(argv[2]);

    short* ptr = image.data();
    /*for (short i=0; i < IMG_SIZY*3; i++) {
        switch (i) {
            case 0: cout << "\t\033[1;31mRED MATRIX\033[0m\n"; break;
            case IMG_SIZY: cout << "\t\033[1;32mGREEN MATRIX\033[0m\n"; break;
            case IMG_SIZY*2: cout << "\t\033[1;34mBLUE MATRIX\033[0m\n"; break;
        }
        for (short v=0; v < IMG_SIZX; v++) {
            if (ptr[i*IMG_SIZX + v] < 10)
                cout << "  " << ptr[i*IMG_SIZX + v] << ' ';
            else if (ptr[i*IMG_SIZX + v] < 100)
                cout << " " << ptr[i*IMG_SIZX + v] << ' ';
            else
                cout << ptr[i*IMG_SIZX + v] << ' ';
        }
        cout << '\n';
    }*/

    array<short, RESULT_SIZE> red;
    array<short, RESULT_SIZE> gre;
    array<short, RESULT_SIZE> blu;
    applyFilter(ptr, kernel, red, gre, blu);
    cout << total_time << '\n';

    CImg<short> new_image(result_x_size, result_y_size, 1, 3);
    
    writeRGBtoImg(new_image, red, gre, blu);

    //image.display();
    //new_image.display();
    argv[1] += 5;// drop the "img/" from the path
    std::string save_path = "output/" + std::string(argv[1]);
    new_image.save(save_path.c_str());

    return 0;
}

template <size_t N>
void convolution(const short* ptr, const array<short, KERNEL_SIZE>& kernel, array<short, N>& result) {
    ushort x_moves = IMG_SIZX - KERNEL_X + 1;
    ushort y_moves = IMG_SIZY - KERNEL_Y + 1;

    double start, finish;
    GET_TIME(start);
    #pragma omp parallel for num_threads(thread_count) schedule(static, 2)
    for (ushort i=0; i < y_moves; i++) {
        for (ushort v=0; v < x_moves; v++) {
            result[i*x_moves + v] = kernel_calc(ptr, i, v, kernel);
        }
    }
    GET_TIME(finish);
    total_time += finish-start;
}

short kernel_calc(const short* ptr, short x_offset, short y_offset, const array<short, KERNEL_SIZE>& kernel) {
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
                cout << "  " << matrix[i*x_dimension + v] << ' ';
            else if (matrix[i*x_dimension + v] <100)
                cout << " " << matrix[i*x_dimension + v] << ' ';
            else
                cout << matrix[i*x_dimension + v] << ' ';
        }
        cout << '\n';
    }
}

template <size_t N>
void applyFilter(short* ptr, const array<short, KERNEL_SIZE>& kernel, array<short, N>& red, array<short, N>& green, array<short, N>& blue) {
    convolution(ptr, kernel, red);
    ptr += IMAGE_CHANNEL_SIZE;
    convolution(ptr, kernel, green);
    ptr += IMAGE_CHANNEL_SIZE;
    convolution(ptr, kernel, blue);
}

template <size_t N>
void writeRGBtoImg(CImg<short>& new_image, array<short, N>& red, array<short, N>& gre, array<short, N>& blu) {
    for (unsigned short i=0; i < result_y_size; i++) {
        unsigned int inicio = i * result_x_size;
        for (unsigned short v=0; v < result_x_size; v++) {
            new_image[i*result_x_size + v] = red[inicio + v];
        }
    }
    for (unsigned short i=result_y_size; i < result_y_size*2; i++) {
        unsigned int inicio = (i-result_y_size) * result_x_size;
        for (short v=0; v < result_x_size; v++) {
            new_image[i*result_x_size + v] = gre[inicio + v];
        }
    }
    for (unsigned short i=result_y_size*2; i < result_y_size*3; i++) {
        unsigned int inicio = (i-result_y_size*2) * result_x_size;
        for (unsigned short v=0; v < result_x_size; v++) {
            new_image[i*result_x_size + v] = blu[inicio + v];
        }
    }
}