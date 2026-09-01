#include "image.h"
#include <cassert>
#include <utility>
#include <cstring>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>

Image::Image(std::string path) : Signal(true)
{
    data = stbi_load(path.c_str(), &size_x, &size_y, &channels, 0);
    original_to_modified();
}

Image::Image(const char* path) : Signal(true)
{
    data = stbi_load(path, &size_x, &size_y, &channels, 0);
    original_to_modified();
}

void Image::original_to_modified()
{
    size = size_x * size_y;
    size_t size_ch = static_cast<size_t>(size) * static_cast<size_t>(channels);
    modified_data = new unsigned char[size_ch];
    std::memcpy(modified_data, data, size_ch);
}

Image::~Image()
{
    send_simple("image_kill",0);
    stbi_image_free(data);
    delete[] modified_data;
}

void Image::add_modifier(Modifier modifier)
{
    modifier_stack.push_back(modifier);
    update_image();
}

void Image::remove_modifier(unsigned int position)
{
    assert(position < modifier_stack.size());
    modifier_stack.erase(modifier_stack.begin() + position);
    update_image();
}
void Image::swap_modifier_position(int original_position,int new_position)
{

    if (original_position >= modifier_stack.size() || new_position >= modifier_stack.size() || new_position < 0)
    {
        std::cout << "Positions O and N: " << original_position << " | " << new_position << std::endl;
        return;
    }
    std::swap(modifier_stack[original_position],modifier_stack[new_position]);
    update_image();
}

unsigned char* Image::get_data()
{
    return data;
}

unsigned char* Image::get_modified_data()
{
    return modified_data;
}

void Image::modify_modifier(unsigned int index,Modifier modifier)
{
    modifier_stack[index] = modifier;
    update_image();
}

void Image::update_image()
{
    original_to_modified();

    for (Modifier mod : modifier_stack)
    {
        if (!mod.visible)
        {
            continue;
        }
        switch (mod.type)
        {
            case MOD_GRAYSCALE:
                ImageFunc::grayscale(modified_data, size_x, size_y, channels);
                break;
            case MOD_QUANTIZE:
                ImageFunc::quantize(modified_data, size_x, size_y, channels, mod.data_a);
                break;
            case MOD_FLIP_H:
                ImageFunc::flip_horizontal(modified_data, size_x, size_y, channels);
                break;
            case MOD_FLIP_V:
                ImageFunc::flip_vertical(modified_data, size_x, size_y, channels);
                break;
        }
    }

    send_simple("image_update",0);
}

void Image::receive_simple(std::string message, int num)
{
    if (message != "add_modifier")
    {
        return;
    }
    //std::cout << "Adding modifier: " << num << std::endl;
    switch(num)
    {
        case MOD_GRAYSCALE:
            add_modifier({MOD_GRAYSCALE, 0,0,0,""});
            break;
        case MOD_QUANTIZE:
            add_modifier({MOD_QUANTIZE, 8,0,0,""});
            break;
        case MOD_FLIP_H:
            add_modifier({MOD_FLIP_H,0,0,0,""});
            break;
        case MOD_FLIP_V:
            add_modifier({MOD_FLIP_V,0,0,0,""});
            break;
    }
}

void Image::receive(void* sender, std::string t_sender, void* data, std::string t_data) {}

void Image::save_image(const char* path)
{
    stbi_write_jpg(path, size_x, size_y, channels, get_modified_data(), 100);
}