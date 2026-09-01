#include "main_window.h"
#include "FL/Fl_Valuator.H"
#include <FL/Fl.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <cstring>
#include <iostream>
void MainWindow::create_initial_layout(int w, int h)
{

    Fl_Flex* master_flex = new Fl_Flex(0,0,w,h);

    top_bar = new TopBar(0,0,0,0,this);

    master_flex->fixed(top_bar,30);

    int y_begin = 30;
    h = h - 30;

    tile = new Fl_Tile(0,y_begin,w,h);

    int panel_size = (int)((float)w * 0.3);
    int img_size = w - panel_size;

    /*side_panel_flex = new Fl_Flex(0,y_begin,panel_size,h,Fl_Flex::VERTICAL);
    side_panel_flex->box(FL_DOWN_BOX);*/
    mod_ui = new ModifierUI(nullptr,0,y_begin,panel_size,h);
    mod_ui->end();
    /*side_panel_flex->resizable(mod_ui);
    side_panel_flex->end();*/
    tile->size_range(mod_ui, 200, 50);

    image_panel = new Fl_Flex(panel_size,y_begin,img_size,h);
    image_panel->type(FL_HORIZONTAL);
    //image_panel->color(FL_BLACK);
    image_panel->box(FL_UP_BOX);
    original_ibox = new ImageBox(0,0,img_size,h);
    modified_ibox = new ImageBox(0,0,img_size,h);
    //image_box->color(FL_BLACK);
    image_panel->end();
    tile->size_range(image_panel, 50, 50);
    
    tile->resizable(image_panel);
    tile->end();
    
    master_flex->end();
    this->resizable(master_flex);
    this->size_range(400,200);
}


const char* MainWindow::choose_image_file()
{
    Fl_Native_File_Chooser fc;
    fc.title("Open image");
    fc.type(Fl_Native_File_Chooser::BROWSE_FILE);
    fc.filter("*.{png,jpg,jpeg}");
    fc.directory("/home/victfv/Documents/Facul/ImgProc/");

    switch (fc.show())
    {
        case -1: return ("Err"); break;
        case  1: return ("Canceled"); break;
        default: return fc.filename();
    }
    return fc.filename();
}

const char* MainWindow::save_image_file()
{
    Fl_Native_File_Chooser fc;
    fc.title("Open image");
    fc.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
    fc.filter("*.{jpg,jpeg}");
    fc.directory("/home/victfv/Documents/Facul/ImgProc/");
    fc.options(Fl_Native_File_Chooser::SAVEAS_CONFIRM);
    switch (fc.show())
    {
        case -1: /*puts(fc.errmsg())*/return "Err"; break;
        case  1: return "Canceled"; break;
        default: return fc.filename();;
    }
    
}

void MainWindow::open_image()
{
    const char* path = choose_image_file();
    if (std::strcmp(path,"Canceled") == 0 || std::strcmp(path,"Err") == 0)
    {
        puts("Cancel or err");
        return;
    }

    if (proc_image != nullptr)
    {
        delete proc_image;
        proc_image = nullptr;
    }


    proc_image = new Image(path);
    mod_ui->set_image(proc_image);

    update_image();
}

void MainWindow::update_image()
{
    original_ibox->set_image(proc_image, true);
    modified_ibox->set_image(proc_image, false);
}

void MainWindow::save_image()
{
    if (proc_image == nullptr)
    {
        return;
    }
    const char* path = save_image_file();

    if (std::strcmp(path,"Canceled") == 0 || std::strcmp(path,"Err") == 0)
    {
        puts("Cancel or err");
        return;
    }

    proc_image->save_image(path);

}

void MainWindow::receive_simple(std::string message, int num)
{
    if (message == "image_update" && proc_image != nullptr)
    {
        update_image();
    }
    else if (message == "toggle_image")
    {
        switch(num)
        {
            case 0:
                original_ibox->show();
                modified_ibox->show();
                image_panel->fixed(original_ibox,0);
                image_panel->fixed(modified_ibox,0);
                break;
            case 1:
                original_ibox->show();
                modified_ibox->hide();
                image_panel->fixed(original_ibox,0);
                image_panel->fixed(modified_ibox,1);
                break;
            case 2:
                original_ibox->hide();
                modified_ibox->show();
                image_panel->fixed(original_ibox,1);
                image_panel->fixed(modified_ibox,0);
                break;
        }
        redraw();
    }
}