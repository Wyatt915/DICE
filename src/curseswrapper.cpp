#include "curseswrapper.hpp"
#include <ncurses.h>

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

void end_curses(){
    endwin();
}
