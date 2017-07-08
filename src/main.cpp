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
        std::vector<skill> myList;
        skill temp;
        db << "SELECT name,base,diff,pnts FROM skills"
            >> [&](std::string name, std::string base, std::string diff, int points){
                temp.name = name;
                temp.base = base;
                temp.pnts = points;
                if(diff == "EASY"){ temp.diff = easy; }
                if(diff == "AVERAGE"){ temp.diff = average; }
                if(diff == "HARD"){ temp.diff = hard; }
                if(diff == "VERY HARD"){ temp.diff = veryhard; }
                myList.push_back(temp);
            };

        WINDOW * win1 = newwin(SCREENH, SCREENW/3, 0, 0);
        WINDOW * win2 = newwin(SCREENH, SCREENW/3, 0, SCREENW/3 + 1);
        WINDOW * win3 = newwin(SCREENH, SCREENW/3, 0, (2 * SCREENW/3) + 1);
        ListSkills l1(& db, win1, myList);
        ListSkills l2(& db, win2, myList);
        ListSkills l3(& db, win3, myList);
        ListSkills views[3] = {l1, l2, l3};
        int c;
        int count = 0;
        doupdate();
        Roller r;
        //r.update();
        //r.hide();
        views[0].update();
        views[1].update();
        views[2].update();
        views[0].give_focus();
        update_panels();
        //refresh();
        doupdate();
        while((c = getch()) != 'q'){
            if(c == '\t'){
                views[count%3].revoke_focus();
                count++;
                views[count%3].give_focus();
            }
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
                views[count%3].process(c);
                doupdate();
                views[count%3].update();
            }
            //refresh();
        }

        //l.listen();
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
