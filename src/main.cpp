#include "editor.hpp"
#include "curseswrapper.hpp"
#include "listview.hpp"
#include "roller.hpp"

#include <sqlite_modern_cpp.h>
#include <string>
#include <vector>
#include <iostream>
#include <ncurses.h>

int SCREENH, SCREENW;

void rollertest(){
    refresh();
    Roller r;
    r.update();
    r.listen();
}

void lvtest(){
    try{
        sqlite::database db("fruit.db");
        std::vector<fruit> myList;
        fruit temp;
        db << "SELECT name,description FROM fruits"
            >> [&](std::string name, std::string desc){
                temp.name = name;
                temp.desc = desc;
                myList.push_back(temp);
            };

        WINDOW * win = newwin(18, 15, 1, 1);
        ListView l(& db, win, myList);
        l.update();
        l.listen();
    }
    catch(std::exception& e){
        std::cout << e.what();
    }
}

int main(int argc, char* argv[]){
    start_curses();
    set_escdelay(10);
    getmaxyx(stdscr, SCREENH, SCREENW);
    
    //rollertest();
    lvtest();

    end_curses();
    return 0;
}
