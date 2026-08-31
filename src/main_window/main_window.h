#ifndef PROC_MAIN_WINDOW
#define PROC_MAIN_WINDOW

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Tile.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_RGB_Image.H>
#include "top_bar.h"
#include "../image_processing/image.h"
#include "../modifier_ui/modifier_ui.h"
#include "../signals/signals.h"

#include <string>

class TopBar;

class ImageBox : public Fl_Box
{
    Fl_RGB_Image* original = nullptr;
    Fl_RGB_Image* scaled = nullptr;

public:
    ImageBox(int X, int Y, int W, int H)
        : Fl_Box(X, Y, W, H)
    {
        box(FL_NO_BOX);
    }

    void set_image(Fl_RGB_Image* img)
    {
        original = img;
        update_image();
        redraw();
    }

    static void resize_timeout(void* userdata)
    {
        auto* box = static_cast<ImageBox*>(userdata);
        box->update_image();
    }

    void resize(int X, int Y, int W, int H) override
    {
        Fl_Box::resize(X, Y, W, H);

        if (!Fl::has_timeout(resize_timeout, this))
            Fl::add_timeout(0.008, resize_timeout, this);
    }

    /*void draw() override
    {
        
        //fl_color(FL_BLACK);
        fl_rectf(x(), y(), w(), h());

        if (!scaled)
            return;

        
        int px = x() + (w() - scaled->w()) / 2;
        int py = y() + (h() - scaled->h()) / 2;

        scaled->draw(px, py);
    }*/

private:
    void update_image()
    {
        if (!original || w() <= 0 || h() <= 0)
            return;

        int iw = original->w();
        int ih = original->h();

        float scale = std::min(
            static_cast<float>(w()) / iw,
            static_cast<float>(h()) / ih
        );

        int nw = std::max(1, static_cast<int>(iw * scale));
        int nh = std::max(1, static_cast<int>(ih * scale));

        delete scaled;
        scaled = static_cast<Fl_RGB_Image*>(original->copy(nw, nh));

        image(scaled);
        Fl::redraw();
    }
};

class MainWindow : public Fl_Double_Window, public Signal
{
    public:
        MainWindow(int w, int h, const char* label = nullptr) : Fl_Double_Window(w,h,label), Signal(true)
        {
            create_initial_layout(w,h);
        }
        
        void open_image();
        void save_image();
        
        Image* get_image();

        void receive_simple(std::string message, int num);
        void receive(void* sender, std::string t_sender, void* data, std::string t_data){};
    private:
        void choose_image_file_callback(Fl_Widget* w, void* user_data)
        {
            MainWindow* window = static_cast<MainWindow*>(user_data);
            window->choose_image_file();
        }

        void save_image_file_callback(Fl_Widget* w, void* user_data)
        {
            MainWindow* window = static_cast<MainWindow*>(user_data);
            window->save_image_file();
        }

        void create_initial_layout(int w, int h);
        
        static const char* choose_image_file();
        static const char* save_image_file();

        void create_fl_image(unsigned char* data, int w, int h, int channels);
        void update_image();

        //Fl_Tabs* tabs;
        Fl_Flex* image_panel;
        ImageBox* image_box;
        Fl_RGB_Image* main_fl_image = nullptr;
        Fl_Flex* side_panel_flex;
        TopBar* top_bar;
        ModifierUI* mod_ui;

        Fl_Tile* tile;
        Image* image_proc = nullptr;
};

#endif
