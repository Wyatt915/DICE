#include "editor.hpp"
#include "listview.hpp"
#include "sqlite3.h"

#include <algorithm>
#include <ncurses.h>
#include <panel.h>

ListView::ListView(){
    savefile = nullptr;
    scroll = 0;
    selection = 0;
    has_focus = false;
    std::fill(margin, margin + 4, 1);
    curx = margin[MLFT];
    cury = margin[MTOP];
}

ListView::ListView(sqlite3* db, WinPos def){
    savefile = db;
    lwin = newwin(def.h, def.w, def.y, def.x);
    lpanel = new_panel(lwin);
    getmaxyx(lwin, fieldheight, fieldwidth);
    std::fill(margin, margin + 4, 1);
    curx = margin[MLFT];
    cury = margin[MTOP];
}

ListView::~ListView(){
    //It is VERY IMPORTANT to delete the PANEL FIRST, and the WINDOW LAST.
    del_panel(lpanel);
    delwin(lwin);
}

void ListView::give_focus(){
    has_focus = true;
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
            cury = num_items + margin[MTOP] - 1;
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

void ListView::setFooter(std::string f){
    footer = f;
    has_footer = true;
    margin[MBOT] = 2;
}

void ListView::setHeader(std::string h){
    header = h;
    has_header = true;
    margin[MTOP] = 2;
}

void ListView::setTitle(std::string t){
    title = t;
}

void ListView::show(){
    show_panel(lpanel);
}

void ListView::hide(){
    hide_panel(lpanel);
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
