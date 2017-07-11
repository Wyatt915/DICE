#include "listview.hpp"
#include "editor.hpp"

#include <algorithm>
#include <sqlite_modern_cpp.h>
#include <ncurses.h>
#include <panel.h>

ListView::ListView(){
    //curs_set(0); //invisible cursor
    scroll = 0;
    selection = 0;
    has_focus = false;
    std::fill(margin, margin + 4, 1);
    curx = margin[MLFT];
    cury = margin[MTOP];
}

ListView::ListView(sqlite::database* db, WINDOW* win){
    savefile = db;
    lwin = win;
    lpanel = new_panel(lwin);
    getmaxyx(lwin, fieldheight, fieldwidth);
    std::fill(margin, margin + 4, 1);
    curx = margin[MLFT];
    cury = margin[MTOP];
}

ListView::~ListView(){
    del_panel(lpanel);
    curs_set(1);
}

void ListView::give_focus(){
    has_focus = true;
    curs_set(0);
}

void ListView::revoke_focus(){
    has_focus = false;
}

void ListView::listen(){
    int c;
    while((c = getch()) != KEY_F(1)){
        process(c);
    }
}

bool ListView::move_up(){
    if(!has_focus){ return false; } // only move if the window has focus
    //if we are at the top of the list and try to move up, wrap to the bottom.
    if(selection == 0){
        selection = num_items - 1;
        scroll = num_items - fieldheight;
        //if the length of the list does not exceed the fieldheight, do not scroll.
        if(scroll < 0){
            scroll = 0;
            cury = num_items + margin[MTOP];
        }
        else{
            cury = margin[MTOP] + fieldheight;
        }
    }
    //if we are at the top of the window, but not the top of the list
    else if(cury == margin[MTOP]){
        selection--;
        scroll--;
    }
    else{
        cury--;
        selection--;
    }
    wmove(lwin, cury, curx);
    return true;
}

bool ListView::move_down(){
    if(!has_focus){ return false; } // only move if the window has focus
    //if we are at the bottom of the list, jump to the top
    if(selection == num_items - 1){
        scroll = 0;
        selection = 0;
        cury = margin[MTOP];
    }
    //If we are at the bottom of the window, but not the bottom of the list
    else if(cury == margin[MTOP] + fieldheight){
        scroll++;
        selection++;
    }
    else{
        cury++;
        selection++;
    }
    wmove(lwin, cury, curx);
    return true;
}
