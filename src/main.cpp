#include "editor.hpp"
#include "curseswrapper.hpp"
#include "listview.hpp"
#include <sqlite_modern_cpp.h>
#include <string>
#include <vector>
#include <iostream>
#include <ncurses.h>


int main(int argc, char* argv[]){
    try{
        std::string thermo;        
        sqlite::database db("fruit.db");
        std::vector<fruit> myList;
        fruit temp;
        db << "SELECT name,description FROM fruits"
            >> [&](std::string name, std::string desc){
                temp.name = name;
                temp.desc = desc;
                myList.push_back(temp);
            };
        start_curses();
        set_escdelay(10);
        int h,w;
        getmaxyx(stdscr, h, w);
        WINDOW * win = newwin(15, 15, 1, 1);
        ListView l(& db, win, myList);
        l.update();
        //refresh();
        //wrefresh(win);
        l.listen();
        wrefresh(win);
        delwin(win);
        //Editor ed(thermo);
        //ed.update();
        //ed.listen();
        end_curses();
        //std::cout << ed.toString();
    }
    catch(std::exception& e){
        std::cout << e.what() <<"\n";
    }

    return 0;
}
