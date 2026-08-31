#include "modifier_ui.h"
#include <iostream>
#include <FL/Fl_Button.H>
#include <FL/Fl_Toggle_Button.H>

ModifierUIItem::ModifierUIItem(Modifier modifier, unsigned int index) : Fl_Flex(0,0,20,20)
{
    mod = modifier;
    idx = index;
    this->box(FL_UP_BOX);
    this->type(HORIZONTAL);
    //std::cout << "Modifier type in UI: " << modifier.type << std::endl; 
    Fl_Flex* vflex = new Fl_Flex(0,0,0,0);
    this->fixed(vflex,24);
    Fl_Flex* flx = new Fl_Flex(0,0,0,0);
    flx->end();
    Fl_Toggle_Button* visible_btn = new Fl_Toggle_Button(16,16,16,16,"@+");
    visible_btn->value(mod.visible);
    visible_btn->callback(visible_callback,static_cast<void*>(this));
    vflex->fixed(visible_btn,24);
    flx = new Fl_Flex(0,0,0,0);
    flx->end();
    vflex->end();
    Fl_Flex* subflex = new Fl_Flex(VERTICAL);
    Fl_Box* box = new Fl_Box(0,0,0,0);
    switch (modifier.type)
    {
        case MOD_GRAYSCALE:
            box->copy_label("Grayscale");
            break;
        case MOD_QUANTIZE:
            {
                box->copy_label("Quantize");
                std::string lvls = "Levels: " + std::to_string(modifier.data_a);
                slider = new Fl_Hor_Slider(0,0,0,0);
                slider->copy_label(lvls.c_str());
                slider->align(FL_ALIGN_TOP);
                slider->range(2,64);
                slider->value(static_cast<double>(modifier.data_a));
                slider->precision(0);
                slider->when(FL_WHEN_RELEASE);
                slider->callback(slider_callback, static_cast<void*>(this));
                this->fixed(slider,18);
            }
            break;
        case MOD_FLIP_H:
            {
                box->copy_label("Horizontal Flip");
            }
            break;
        case MOD_FLIP_V:
            {
                box->copy_label("Vertical Flip");
            }
            break;
    }
    subflex->end();
    subflex = new Fl_Flex(VERTICAL);
    Fl_Button* close_btn = new Fl_Button(0,0,0,0,"X");
    close_btn->callback(delete_callback, static_cast<void*>(this));
    Fl_Button* up_btn = new Fl_Button(0,0,0,0,"^");
    up_btn->callback(up_callback, static_cast<void*>(this));
    if (idx == 0)
        up_btn->deactivate();

    Fl_Button* down_btn = new Fl_Button(0,0,0,0,"v");
    down_btn->callback(down_callback, static_cast<void*>(this));
    Image* image = ModifierUI::instance->get_image();
    if (idx == image->get_modifier_count() - 1)
        down_btn->deactivate();

    this->fixed(subflex, 32);
    subflex->end();
    this->end();
}

ModifierUIItem::~ModifierUIItem()
{
    this->clear();
}
void ModifierUIItem::slider_callback(Fl_Widget* w, void* user_data)
{
    ModifierUIItem* ui_item = static_cast<ModifierUIItem*>(user_data);
    Image* image = ModifierUI::instance->get_image();
    ui_item->mod.data_a = static_cast<int>(ui_item->slider->value());
    image->modify_modifier(ui_item->idx,ui_item->mod);
}

void ModifierUIItem::up_callback(Fl_Widget* w, void* user_data)
{
    ModifierUIItem* ui_item = static_cast<ModifierUIItem*>(user_data);
    Image* image = ModifierUI::instance->get_image();
    image->swap_modifier_position(ui_item->idx, ui_item->idx - 1);
}

void ModifierUIItem::down_callback(Fl_Widget* w, void* user_data)
{
    ModifierUIItem* ui_item = static_cast<ModifierUIItem*>(user_data);
    Image* image = ModifierUI::instance->get_image();
    image->swap_modifier_position(ui_item->idx, ui_item->idx + 1);
}

void ModifierUIItem::delete_callback(Fl_Widget* w, void* user_data)
{
    ModifierUIItem* ui_item = static_cast<ModifierUIItem*>(user_data);
    Image* image = ModifierUI::instance->get_image();
    image->remove_modifier(ui_item->idx);
}

void ModifierUIItem::visible_callback(Fl_Widget* w, void* user_data)
{
    ModifierUIItem* ui_item = static_cast<ModifierUIItem*>(user_data);
    Image* image = ModifierUI::instance->get_image();
    Fl_Toggle_Button* btn = static_cast<Fl_Toggle_Button*>(w);
    Modifier md = ui_item->mod;
    md.visible = btn->value() != 0;
    image->modify_modifier(ui_item->idx, md);
}

ModifierUI* ModifierUI::instance;

ModifierUI::ModifierUI(Image* img, int x, int y, int w, int h) : Fl_Flex(x,y,w,h), Signal(true)
{
    instance = this;
    this->box(FL_DOWN_BOX);
    image = img;
}

void ModifierUI::update_list()
{
    this->clear();
    if (image == nullptr)
    {
        return;
    }
    std::vector<Modifier> modifiers = image->get_modifiers();
    this->begin();
    for (unsigned int i = 0; i < modifiers.size(); i++)
    {
        ModifierUIItem* current = new ModifierUIItem(modifiers[i], i);
        this->fixed(current, ITEM_HEIGHT);
    }
    this->end();
    redraw();
}

void ModifierUI::set_image(Image* img)
{
    image = img;
    update_list();
}

void ModifierUI::receive_simple(std::string message, int num)
{
    if (message == "image_update")
    {
        std::cout << "Modifier received: " << message << std::endl;
        update_list();
    }
    else if (message == "image_kill")
    {
        image = nullptr;
        update_list();
    }
}