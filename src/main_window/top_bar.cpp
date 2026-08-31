#include "top_bar.h"
#include <stdint.h>
#include "main_window.h"
#include "../image_processing/image.h"

/*const Fl_Menu_Item bar_items[4] =
        {
            {"File", FL_ALT+'f' ,                0, 0, FL_SUBMENU},
            {"Open", 0          , TopBar::submenu_callback, (void*)0},
            {"Save", 0          , TopBar::submenu_callback, (void*)1},
            {0}
        };*/

TopBar::TopBar(int x, int y, int w, int h, MainWindow* mw, const char*l) : Fl_Menu_Bar(x,y,w,h,l), Signal()
{
    main_window = mw;
    instance = this;
    create_bar();
}

TopBar* TopBar::instance;
MainWindow* TopBar::main_window;

void TopBar::create_bar()
{
    this->add("File"                    ,FL_ALT+'f'  ,0,0,FL_SUBMENU);
    this->add("File/Open"               ,0           ,submenu_callback ,(void*)0);
    this->add("File/Save"               ,0           ,submenu_callback ,(void*)1);
    this->add("Modifiers"               ,FL_ALT+'m'  ,0,0,FL_SUBMENU);
    this->add("Modifiers/Grayscale"     ,0           ,modifier_callback,(void*)MOD_GRAYSCALE);
    this->add("Modifiers/Quantize"      ,0           ,modifier_callback,(void*)MOD_QUANTIZE);
    this->add("Modifiers/Flip"          ,0           ,0,0,FL_SUBMENU);
    this->add("Modifiers/Flip/Flip&H"   ,0           ,modifier_callback,(void*)MOD_FLIP_H);
    this->add("Modifiers/Flip/Flip&V"   ,0           ,modifier_callback,(void*)MOD_FLIP_V);
    //this->add({0});
}

void TopBar::submenu_callback(Fl_Widget* w, void* data)
{
    intptr_t subm = (intptr_t)data;
    switch(subm)
    {
        case 0:
            main_window->open_image();
            break;
        case 1:
            main_window->save_image();
            break;
        case 3:
            break;
    }
}

void TopBar::modifier_callback(Fl_Widget* w, void* data)
{
    intptr_t subm = (intptr_t)data;
    send_simple("add_modifier", subm);
}