#include "editor.hpp"
#include "curseswrapper.hpp"
#include "listview.hpp"
#include "sqlite_modern_cpp.h"
#include <string>
#include <vector>
#include <iostream>
#include <ncurses.h>

int main(int argc, char* argv[]){
    try{
        //std::string thermo;        
        //sqlite::database db("test.db");
        //db << "SELECT * FROM wew WHERE ROWID=1" >> thermo;
        start_curses();        
        int h,w;
        getmaxyx(stdscr, h, w);
        WINDOW * win = newwin(h/2, w/2, h/4, w/4);
        
        std::vector<std::string> myList = {"Apples", "Bananas", "Carrots", "Durians", "Eggplants", "Frankenberries", "Goldberries"};
        ListView l(win, myList);
        l.update();
        l.listen();
        wrefresh(win);
        end_curses();
        //std::cout << ed.toString();
    }
    catch(std::exception& e){
        std::cout << e.what() <<"\n";
    }

    return 0;
}
