#include "editor.hpp"
#include "listview.hpp"
#include "sqlite3.h"

#include <algorithm>
#include <ncurses.h>
#include <panel.h>

extern sqlite3* savefile;

ListView::ListView():DiceWin(){
    selection = 0;
    defaultcursor = 0;
}

ListView::ListView(WinPos def):DiceWin(def){
    selection = 0;
    defaultcursor = 0;
}

ListView::~ListView(){
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
    wmove(win, cury, curx);
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
    wmove(win, cury, curx);
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

void ListView::update(){
    wmove(win, 0, 0);
    wclrtobot(win);
    
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    
    if(has_header){
        mvwaddstr(win, margin[MLFT], 1, header.c_str());
    }
    if(has_footer){
        mvwaddstr(win, margin[MLFT], fieldheight - 1, footer.c_str());
    }
    
    int i;
    for(i = 0; i + scroll < num_items && i - scroll < fieldheight; i++){
        wmove(win, i + margin[MTOP] + scroll, margin[MLFT]);
        if(i + scroll == selection && has_focus){
            wattron(win, A_UNDERLINE | A_STANDOUT);
            waddstr(win, contents[i+scroll][0].c_str());
            wattroff(win, A_UNDERLINE | A_STANDOUT);
        }
        else{
            waddstr(win, contents[i+scroll][0].c_str());
        }
        for(int j = 1; j < contents.size(); j++){
            wmove(win, i + margin[MTOP] + scroll, margin[MLFT] + tabstops[j] + 1);
            waddstr(win, contents[i+scroll][j].c_str());
        }
        wmove(win, cury, curx);
    }
    
    if(has_focus){ wattron(win, COLOR_PAIR(2)); }
    box(win, 0, 0);
    if(has_focus){ wattroff(win, COLOR_PAIR(2)); }
    
    wnoutrefresh(win);
    update_panels();
    doupdate();
    
    wmove(win, cury, curx);
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
