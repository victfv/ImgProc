#include "main_window/main_window.h"
#include <iostream>

void ButtonCallback(Fl_Widget*, void*)
{
    puts("Hello!");
}

int main(int argc, char **argv) {
    MainWindow *window = new MainWindow(800, 600, "ImgProc");
    

    window->end();
    window->show();

    return Fl::run();
}