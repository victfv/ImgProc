#ifndef PROC_TOP_BAR
#define PROC_TOP_BAR
#include <FL/Fl.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Menu_Item.H>
#include "../signals/signals.h"

class MainWindow;

class TopBar : public Fl_Menu_Bar, public Signal
{
    public:
        TopBar(int x, int y, int w, int h, MainWindow* mw, const char*l=0);

        void receive_simple(std::string message, int num){}
        void receive(void* sender, std::string t_sender, void* data, std::string t_data){}
    private:
        void create_bar();

        static TopBar* instance;

        static void submenu_callback(Fl_Widget* w, void* data);
        static void modifier_callback(Fl_Widget* w, void* data);
        
        static MainWindow* main_window; 
        /*Fl_Menu_Item bar_items[4] =
        {
            {"File", FL_ALT+'f' ,                0, 0, FL_SUBMENU},
            {"Open", 0          , submenu_callback, (void*)0},
            {"Save", 0          , submenu_callback, (void*)1},
            {0}
        };*/
};

#endif