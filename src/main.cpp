#include "editor.hpp"
#include "curseswrapper.hpp"

#include "sqlite_modern_cpp.h"
#include <string>
//#include <vector>
#include <iostream>

int main(int argc, char* argv[]){
    try{
        std::string thermo = "Thermodynamics is a branch of physics concerned with heat and temperature and their relation to energy and work. The behavior of these quantities is governed by the four laws of thermodynamics, irrespective of the composition or specific properties of the material or system in question. The laws of thermodynamics are explained in terms of microscopic constituents by statistical mechanics. Thermodynamics applies to a wide variety of topics in science and engineering, especially physical chemistry, chemical engineering and mechanical engineering."; 
        
        sqlite::database db("test.db");
        //db << "SELECT * FROM wew;" >> thermo;
        
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
