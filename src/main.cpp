#include "editor.hpp"
#include "curseswrapper.hpp"
#include "list_skills.hpp"
#include "roller.hpp"
#include "parse.hpp"

#include <sqlite_modern_cpp.h>
#include <string>
#include <vector>
#include <iostream>
#include <ncurses.h>
#include <panel.h>

int SCREENH, SCREENW;

void rollertest(){
    refresh();
    Roller r;
    r.update();
    r.listen();
}

void lvtest(){
    try{
        sqlite::database db("skills.db");
        WINDOW * win1 = newwin(SCREENH, SCREENW/3, 0, 0);
        ListSkills l1(& db, win1);
        l1.show();
        l1.give_focus();
        Roller r;
        r.hide();
        l1.update();
        refresh();
        update_panels();
        doupdate();
        l1.update();
        int c;
        while((c = getch()) != 'q'){
            if(c == ' '){
                curs_set(1);
                r.show();
                r.roll("3d6");
                r.update();
                r.listen();
                r.hide();
                curs_set(0);
                update_panels();
                refresh();
            }
            else{
                l1.process(c);
            }
        }
        delwin(win1);
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
