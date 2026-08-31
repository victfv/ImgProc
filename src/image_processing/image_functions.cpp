#include "image_functions.h"
#include <cmath>
#include <cstring>
#include <algorithm>
#include <iostream>

namespace ImageFunc
{
    void rgb_to_hsv(unsigned char* original, unsigned int wid, unsigned int hei, int channels);
    void hsv_to_rgb(double* hsv, unsigned int wid, unsigned int hei, int channels, unsigned char* buff);
    void dup_to_buffer(unsigned char* original, unsigned int w, unsigned int h, int channels, unsigned char* buff);

    unsigned char* temp_buffer_a = nullptr;
    unsigned char* temp_buffer_b = nullptr;
    double* hsv_buffer = nullptr;

    unsigned int xy_to_index(unsigned int x, unsigned int y,unsigned int w, unsigned int h, int channels)
    {
        return (y * w + x) * channels;
    }

    void flip_horizontal(unsigned char* image, unsigned int w, unsigned int h, int channels)
    {
        
        unsigned int h_width = (w / 2);
        #pragma omp parallel for
        for (unsigned int y = 0; y < h; y++)
        {
            for (unsigned int x = 0; x < h_width; x++)
            {
                unsigned int index_left = xy_to_index(x,y,w,h,channels);
                unsigned int index_right = xy_to_index(w - x - 1,y,w,h,channels);
                unsigned char temp[channels];
                for (int c = 0; c < channels; c++)
                {
                    temp[c] = image[index_left + c];
                }

                for (int c = 0; c < channels; c++)
                {
                    image[index_left + c] = image[index_right + c];
                    image[index_right + c] = temp[c];
                }
            }
        }
    }
    void flip_vertical(unsigned char* image, unsigned int w, unsigned int h, int channels)
    {
        unsigned int h_height = (h / 2);
        unsigned int line_size = sizeof(unsigned char) * w * channels;
        #pragma omp parallel for
        for (unsigned int i = 0; i < (h_height); i++)
        {
            unsigned int start_offset = i * w * channels;
            unsigned int end_offset = (h - i - 1) * w * channels;
            unsigned char* temp = new unsigned char[w * channels]; 
            std::memcpy(temp, image + start_offset, line_size);
            std::memcpy(image + start_offset, image + end_offset, line_size);
            std::memcpy(image + end_offset, temp, line_size);
        }
    }

    void grayscale(unsigned char* image, unsigned int w, unsigned int h, int channels)
    {
        if (channels < 3)
        {
            return;
        }
        unsigned int size = w * h;
        #pragma omp parallel for
        for (unsigned int i = 0; i < size; i++)
        {
            int r_idx = i * channels;
            int g_idx = r_idx + 1;
            int b_idx = g_idx + 1;
            
            unsigned char gray = static_cast<unsigned char>
            (
                image[r_idx] * 0.299 +
                image[g_idx] * 0.578 +
                image[b_idx] * 0.114
            );

            image[r_idx] = gray;
            image[g_idx] = gray;
            image[b_idx] = gray;
        }
    }

    void quantize(unsigned char* image, unsigned int w, unsigned int h, int channels, int levels)
    {
        unsigned int size = w * h;
        rgb_to_hsv(image, w, h, channels);
    
        

        unsigned char min_c_v = 255;
        unsigned char max_c_v = 0;
        double min_v = 2.0;
        double max_v = 0.0;

        #pragma omp parallel for
        for (unsigned int i = 0; i < size; i++)
        {   
            unsigned int v_idx = i * 4 + 2;
            double v = hsv_buffer[v_idx];
            unsigned char scaled_v = static_cast<unsigned char>(std::round(v * 255.0));
            
            if (v > max_v)
            {
                max_c_v = scaled_v;
                max_v = v;
            }
            else if (v < min_v)
            {
                min_c_v = scaled_v;
                min_v = v;
            }
        }

        unsigned char delta_c = max_c_v - min_c_v;
        if (delta_c <= levels)
        {
            return;
        }

        double delta = max_v - min_v;
        double half_step = (delta / static_cast<double>(levels)) * 0.5;
        #pragma omp parallel for
        for (unsigned int i = 0; i < size; i++)
        {   
            unsigned int v_idx = i * 4 + 2;
            double v = hsv_buffer[v_idx];

            hsv_buffer[v_idx] = (std::floor((v + half_step) * levels) / levels);
        }

        hsv_to_rgb(hsv_buffer, w, h, channels, image);
    }

    void dup_to_temp_a(unsigned char* original, unsigned int w, unsigned int h, int channels)
    {
        if (temp_buffer_a != nullptr)
        {
            delete[] temp_buffer_a;
        }
        dup_to_buffer(original,w,h,channels,temp_buffer_a);
    }

    void dup_to_temp_b(unsigned char* original, unsigned int w, unsigned int h, int channels)
    {
        if (temp_buffer_b != nullptr)
        {
            delete[] temp_buffer_b;
        }
        dup_to_buffer(original,w,h,channels,temp_buffer_b);
    }

    void dup_to_buffer(unsigned char* original, unsigned int w, unsigned int h, int channels, unsigned char* buff)
    {
        if (buff != nullptr)
        {
            delete[] buff;
        }
        size_t size = w * h * channels;
        buff = new unsigned char[size];
        std::memcpy(buff, original, size);
    }

    const double zero_threshold = 0.000001;
    void rgb_to_hsv(unsigned char* original, unsigned int wid, unsigned int hei, int channels)
    {
        if (hsv_buffer != nullptr)
        {
            delete[] hsv_buffer;
        }
        if (channels == 2)
        {
            return;
        }
        unsigned int size = wid * hei;
        hsv_buffer = new double[size * 4];
        if (channels == 1)
        {
            #pragma omp parallel for
            for (unsigned int i = 0; i < size; i++)
            {
                unsigned int h_idx = i * 4;
                unsigned int s_idx = h_idx + 1;
                unsigned int v_idx = s_idx + 1;
                unsigned int hsv_a_idx = v_idx + 1;

                hsv_buffer[h_idx] = 0.0;
                hsv_buffer[s_idx] = 0.0;
                hsv_buffer[v_idx] = static_cast<double>(original[i]) / 255.0;
                hsv_buffer[hsv_a_idx] = 1.0;
            }
            return;
        }
        #pragma omp parallel for
        for (unsigned int i = 0; i < size; i++)
        {
            unsigned int r_idx = i * channels;
            unsigned int g_idx = r_idx + 1;
            unsigned int b_idx = g_idx + 1;
            unsigned int a_idx = b_idx + 1;

            unsigned int h_idx = i * 4;
            unsigned int s_idx = h_idx + 1;
            unsigned int v_idx = s_idx + 1;
            unsigned int hsv_a_idx = v_idx + 1;
            
            double r = static_cast<double>(original[r_idx]) / 255.0;
            double g = static_cast<double>(original[g_idx]) / 255.0;
            double b = static_cast<double>(original[b_idx]) / 255.0;
            double a = channels > 3 ? static_cast<double>(original[a_idx]) / 255.0 : 1.0;
            

            double max = std::max({r, g, b});
            double min = std::min({r, g, b});
            double delta = max - min;

            double h = 0.0;
            double s = 0.0;
            double v = max;

            if (max > 0.0)
                s = delta / max;

            if (delta > 0.0)
            {
                if (max == r)
                {
                    h = (g - b) / delta;

                    if (h < 0.0)
                        h += 6.0;
                }
                else if (max == g)
                {
                    h = (b - r) / delta + 2.0;
                }
                else
                {
                    h = (r - g) / delta + 4.0;
                }

                h /= 6.0;
            }

            hsv_buffer[h_idx] = h;
            hsv_buffer[s_idx] = s;
            hsv_buffer[v_idx] = v;
            hsv_buffer[hsv_a_idx] = a;
        }
    }

    void hsv_to_rgb(double* hsv, unsigned int wid, unsigned int hei, int channels, unsigned char* buff)
    {
        unsigned int size = wid * hei;
        #pragma omp parallel for
        for (unsigned int idx = 0; idx < size; idx++)
        {
            unsigned int h_idx = idx * 4;
            unsigned int s_idx = h_idx + 1;
            unsigned int v_idx = s_idx + 1;
            unsigned int a_idx = v_idx + 1;

            unsigned int r_idx = idx * channels;
            unsigned int g_idx = r_idx + 1;
            unsigned int b_idx = g_idx + 1;
            unsigned int rgb_a_idx = b_idx + 1;
            
            double h = hsv[h_idx] * 6.0;
            double s = hsv[s_idx];
            double v = std::clamp(hsv[v_idx],0.0,1.0);
            double a = hsv[a_idx];

            double i = floorf(h);
            double f = h - i;
            double p = v * (1.0 - s);
            double q = v * (1.0 - s * f);
            double t = v * (1.0 - s * (1.00 - f));

            double r = 0.0;
            double g = 0.0;
            double b = 0.0;

            
            switch(static_cast<int>(i) % 6)
            {
                case 0:
                    {
                        r = v;
                        g = t;
                        b = p;
                    }
                    break;
                case 1:
                    {
                        r = q;
                        g = v;
                        b = p;
                    }
                    break;
                case 2:
                    {
                        r = p;
                        g = v;
                        b = t;
                    }
                    break;
                case 3:
                    {
                        r = p;
                        g = q;
                        b = v;
                    }
                    break;
                case 4:
                    {
                        r = t;
                        g = p;
                        b = v;
                    }
                    break;
                case 5:
                    {
                        r = v;
                        g = p;
                        b = q;
                    }
                    break;
            }
            
            buff[r_idx] = static_cast<unsigned char>(std::min(std::round(r * 255.0),255.0));
            buff[g_idx] = static_cast<unsigned char>(std::min(std::round(g * 255.0),255.0));
            buff[b_idx] = static_cast<unsigned char>(std::min(std::round(b * 255.0),255.0));
            if (channels > 3)
            {
                buff[rgb_a_idx] = static_cast<unsigned char>(std::min(std::round(a * 255.0),255.0));
            }
        }
    }
}