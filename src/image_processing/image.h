#ifndef PROC_IMAGE
#define PROC_IMAGE

#include <string>
#include <vector>
#include "../signals/signals.h"
#include "image_functions.h"

enum {MOD_GRAYSCALE = 0, MOD_QUANTIZE, MOD_FLIP_H, MOD_FLIP_V};

struct Modifier
{
    int type;
    int data_a;
    int data_b;
    int data_c;
    std::string data_d;
    bool visible = true;
};

class Image : public Signal
{
    public:
        Image(std::string path);
        Image(const char* path);
        ~Image();

        void add_modifier(Modifier modifier);
        void remove_modifier(unsigned int position);
        void swap_modifier_position(int original_position,int new_position);
        unsigned char* get_data();
        unsigned char* get_modified_data();
        int get_channels(){return channels;}
        int get_modifier_count(){return modifier_stack.size();}
        void modify_modifier(unsigned int index,Modifier modifier);

        std::vector<Modifier> get_modifiers(){return modifier_stack;}

        void receive_simple(std::string message, int num);
        void receive(void* sender, std::string t_sender, void* data, std::string t_data);
        void save_image(const char* path);
        int w(){return size_x;}
        int h(){return size_y;}
    private:
        unsigned char* data;
        unsigned char* modified_data;
        int size_x;
        int size_y;
        int channels;
        int size;
        std::vector<Modifier> modifier_stack;

        void original_to_modified();

        void update_image();
};

#endif