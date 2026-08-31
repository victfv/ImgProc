#ifndef IMAGE_FUNC
#define IMAGE_FUNC
#include <omp.h>

namespace ImageFunc
{
    void grayscale(unsigned char* image, unsigned int w, unsigned int h, int channels);
    void quantize(unsigned char* image, unsigned int w, unsigned int h, int channels, int levels);
    void flip_horizontal(unsigned char* image, unsigned int w, unsigned int h, int channels);
    void flip_vertical(unsigned char* image, unsigned int w, unsigned int h, int channels);
}

#endif