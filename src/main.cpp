#include "editor.hpp"


//#include "sqlite3.h"
#include <ncurses.h>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

int main(int argc, char* argv[]){
    std::string s = "Thermodynamics is a branch of physics concerned with heat and temperature and their relation to energy and work. The behavior of these quantities is governed by the four laws of thermodynamics, irrespective of the composition or specific properties of the material or system in question. The laws of thermodynamics are explained in terms of microscopic constituents by statistical mechanics. Thermodynamics applies to a wide variety of topics in science and engineering, especially physical chemistry, chemical engineering and mechanical engineering.";
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int h, w;
    int c, cx, cy;
    cx = 0; cy = 0;
    getmaxyx(stdscr, h, w);
    WINDOW * win = newwin(h/2, w/2, h/4, w/4);
	Editor ed(win, s);
    refresh();
    for(int i = 0; i < ed.numlines(); i++){
        wmove(win, i+1, 1);
        waddstr(win, ed.getline(i));
    }
    box(win, 0, 0);
    wrefresh(win);
    int action = -1;
    while((c = getch()) != KEY_F(1)){
        getyx(win, cy, cx);
        
        ed.process(c);

        //refresh();
        //std::stringstream mystr;
        //mystr << "(" << cy << ", " << cx << ")";
        //mvaddstr(0,0,mystr.str().c_str());
        //move(cy, cx);
        //refresh();
        //wrefresh(win);
    }
    delwin(win);
    endwin();
    return 0;
}
