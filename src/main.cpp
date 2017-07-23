#include "editor.hpp"
#include "list_skills.hpp"
#include "parse.hpp"
#include "roller.hpp"
#include "sqlite3.h"

#include <iostream>
#include <ncurses.h>
#include <panel.h>
#include <stdexcept>
#include <string>
#include <vector>

int SCREENH, SCREENW;

sqlite3* savefile;
int total_points;

void rollertest(){
    refresh();
    Roller r;
    r.update();
    r.listen();
}

void lvtest(){
    try{

        WinPos dims;
        dims.x = 0;
        dims.y = 0;
        dims.w = SCREENW/3;
        dims.h = SCREENH;
        
        ListSkills l1(dims);
        l1.show();
        l1.give_focus();
        l1.update();
        
        Roller r;
        r.hide();
        refresh();
        update_panels();
        doupdate();
        int c;
        while((c = getch()) != 'q'){
            if(c == ' '){
                l1.revoke_focus();
                r.show();
                r.roll("3d6");
                r.update();
                r.listen();
                r.hide();
                update_panels();
                refresh();
            }
            else{
                l1.give_focus();
                l1.process(c);
            }
        }
        endwin();
    }
    catch(std::exception& e){
        endwin();
        std::cout << e.what() << "\n";
        getch();
        throw e;
    }
}

void start_curses(){
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);            
    start_color();
    if(can_change_color()){
        init_color(COLOR_BLACK, 137, 173, 192);
        init_color(COLOR_BLUE, 100, 300, 700);
        init_color(COLOR_YELLOW, 941, 776, 454);
    }   
    else{
        move(3,0);
        addstr("Custom colors not supported on this terminal.");
    } 
}

static int init_callback(void* notused, int argc, char** argv, char ** azColName)
{
    total_points = std::stoi(std::string(argv[0]));
    return 0;
}

int main(int argc, char* argv[]){
    start_curses();
    set_escdelay(10);
    getmaxyx(stdscr, SCREENH, SCREENW);
    total_points = 0;
    try{
        int rc = sqlite3_open(argv[1], &savefile);
        if(rc != SQLITE_OK){
            throw std::runtime_error("Error: Could not open database.");
        }
        char* zErrMsg = nullptr;
        std::string sql = "SELECT points FROM character WHERE id=1";
        rc = sqlite3_exec(savefile, sql.c_str(), init_callback, NULL, &zErrMsg);
        if(rc != SQLITE_OK){
            std::string s = zErrMsg;
            sqlite3_free(zErrMsg);
            throw std::runtime_error(s);
        }
        lvtest();
        sqlite3_close(savefile);
    }
    catch(std::exception& e){
        sqlite3_close(savefile);
        endwin();
        std::cout << e.what() << "\n";
        return 1;
    }
    endwin();
    return 0;
}

//////////////////////////////////////////////////////
//                                                  //
//      oooooooooo.    o8o                          //
//      `888'   `Y8b   `"'                          //
//       888      888 oooo   .ooooo.   .ooooo.      //
//       888      888 `888  d88' `"Y8 d88' `88b     //
//       888      888  888  888       888ooo888     //
//       888     d88'  888  888   .o8 888    .o     //
//      o888bood8P'   o888o `Y8bod8P' `Y8bod8P'     //
//                                                  //
//////////////////////////////////////////////////////
