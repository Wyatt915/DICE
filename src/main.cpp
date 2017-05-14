#include "editor.hpp"
#include "curseswrapper.hpp"

#include "sqlite_modern_cpp.h"
#include <string>
//#include <vector>
#include <iostream>

int main(int argc, char* argv[]){
    try{
        std::string thermo;        
        sqlite::database db("test.db");
        db << "SELECT * FROM wew WHERE ROWID=2" >> thermo;
        //initscr();
        //cbreak();
        //noecho();
        //keypad(stdscr, TRUE);
        
        //mouse_interval(0);    

        //dice_colors();

        //int h, w;
        //int c, cx, cy;
        //cx = 0; cy = 0;
        //getmaxyx(stdscr, h, w);
        //WINDOW * win = newwin(h/2, w/2, h/4, w/4);
        start_curses();        
        Editor ed(thermo);
        ed.update();
        ed.listen();
        end_curses();
    }
    catch(std::exception& e){
        std::cout << e.what() <<"\n";
    }
    return 0;
}
