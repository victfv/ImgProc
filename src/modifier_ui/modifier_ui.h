#ifndef PROC_UI_MODIFIER
#define PROC_UI_MODIFIER

#include <FL/Fl.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Hor_Slider.H>
#include <vector>
#include "../image_processing/image.h"
#include "../signals/signals.h"

class ModifierUI;

class ModifierUIItem : public Fl_Flex
{
    public:
        ModifierUIItem(Modifier modifier, unsigned int index);
        ~ModifierUIItem();
    private:
        unsigned int idx;
        Modifier mod;
        Fl_Hor_Slider* slider;

        static void slider_callback(Fl_Widget* w, void* user_data);
        static void up_callback(Fl_Widget* w, void* user_data);
        static void down_callback(Fl_Widget* w, void* user_data);
        static void delete_callback(Fl_Widget* w, void* user_data);
        static void visible_callback(Fl_Widget* w, void* user_data);
};

class ModifierUI : public Fl_Flex, public Signal
{
    public:
        ModifierUI(Image* img = nullptr, int x = 0, int y = 0, int w = 0, int h = 0);

        void update_list();
        void set_image(Image* img);

        void receive_simple(std::string message, int num);
        void receive(void* sender, std::string t_sender, void* data, std::string t_data) {}
        Image* get_image(){return image;}

        static ModifierUI* instance;
    private:
        const int ITEM_HEIGHT = 96;
        Image* image = nullptr;
};


#endif