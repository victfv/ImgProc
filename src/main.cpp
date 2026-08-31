#include "main_window/main_window.h"
#include <FL/Fl.H>

int main(int argc, char **argv) {
    MainWindow *window = new MainWindow(800, 600, "ImgProc");
    

    window->end();
    window->show();

    return Fl::run();
}