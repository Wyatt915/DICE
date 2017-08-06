//window.cpp

#include "window.hpp"

#include <ncurses.h>
#include <panel.h>

DiceWin::DiceWin(){
    int h,w;
    getmaxyx(stdscr, h, w);
    win = newwin(h/2, w/2, h/4, w/4);
    pan = new_panel(win);
    std::fill(margin, margin + 4, 1);
    curx = margin[MLFT];
    cury = margin[MTOP];
    getmaxyx(win, fieldheight, fieldwidth);
    fieldwidth -= margin[MRGT] + margin[MLFT];
    fieldheight-= margin[MTOP] + margin[MBOT];
    has_focus = false;
    scroll = 0;
}

DiceWin::DiceWin(WinPos def){
    win = newwin(def.h, def.w, def.y, def.x);
    pan = new_panel(win);
    std::fill(margin, margin + 4, 1);
    curx = margin[MLFT];
    cury = margin[MTOP];
    getmaxyx(win, fieldheight, fieldwidth);
    fieldwidth -= margin[MRGT] + margin[MLFT];
    fieldheight-= margin[MTOP] + margin[MBOT];
    scroll = 0;
    has_focus = false;
}

DiceWin::~DiceWin(){
    //It is VERY IMPORTANT to delete the PANEL FIRST, and the WINDOW LAST.
    del_panel(pan);
    delwin(win);
}

void DiceWin::give_focus(){
    has_focus = true;
}

void DiceWin::revoke_focus(){
    has_focus = false;
}

void DiceWin::setTitle(std::string t){
    title = t;
}

void DiceWin::show(){
   show_panel(pan);
   update_panels();
   doupdate();
}

void DiceWin::hide(){
    hide_panel(pan);
    update_panels();
    doupdate();
}
