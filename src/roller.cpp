#include "font.hpp"
#include "roller.hpp"
#include "parse.hpp"

#include <algorithm>
#include <string>
#include <vector>
#include <stdexcept>
#include <stdlib.h>
#include <time.h>

void Roller::insert(char c){
    if(c > 31 && c < 127){
        auto pos = expr.begin();
        std::advance(pos, curx - 2);
        expr.insert(pos, c);
        move_right();
    }
}
void Roller::remove(){
    auto pos = expr.begin();
    std::advance(pos, curx - 2);
    if(pos != expr.end()){
        expr.erase(pos);
    }
}

#include <ncurses.h>
#include <panel.h>

Roller::Roller(){
    prev_cursor_vis =0;
    srand(time(NULL));
    int h, w;
    exprpos = 0;
    histpos = 0;
    getmaxyx(stdscr, h, w);
    created = true;
    rollwin = newwin(h/2, w/2, h/4, w/4);
    rollpanel = new_panel(rollwin);
    getmaxyx(rollwin, fieldheight, fieldwidth);
    wmove(rollwin, fieldheight - 1, 1);
    doupdate();
    hide_panel(rollpanel);
    refresh();
}

void Roller::display(int val){
        std::vector<std::string> pretty = font::lines(val);
        int width = pretty[0].length();
        int height = pretty.size();
        
        if(fieldwidth < width || fieldheight < height){
            std::string errstr = "Result is too large and cannot be displayed properly";
            std::runtime_error e(errstr);
            std::string result = std::to_string(val);
            cury = fieldheight / 2;
            curx = (fieldwidth - result.length()) / 2;
            wmove(rollwin, cury, curx);
            waddstr(rollwin, result.c_str());
            throw e;
        }
        
        cury = (fieldheight - height)/2;
        curx = (fieldwidth - width)/2;
        wmove(rollwin, cury, curx);
        for(int i = 0; i < height; i++){
            wmove(rollwin, cury + i, curx);
            waddstr(rollwin, pretty[i].c_str());
        }
}

void Roller::roll(std::string command){
    wmove(rollwin, 1, 1);
    wclrtobot(rollwin);
    try{
        if(command.length() == 0){
            if(history.size() > 0){
                command = history.back();
            }
            else{
                std::runtime_error e("Invalid expression.");
                throw e;
            }
        }
        SyntaxTree s(command);
        int total = s.evaluate();
        if(total < 0) total = 0;
        display(total);
        if(history.size() == 0 || command != history.back()){
            history.push_back(command);
        }
        update();
    }
    catch(std::runtime_error& e){
        wmove(rollwin, fieldheight - 3, 1);
        waddstr(rollwin, e.what());
        expr = "";
        update();
    }
}

Roller::~Roller(){
    del_panel(rollpanel);
    delwin(rollwin);
}

void Roller::show(){
    prev_cursor_vis = curs_set(2);
    show_panel(rollpanel);
}

void Roller::hide(){
    curs_set(prev_cursor_vis);
    hide_panel(rollpanel);
}

void Roller::update(){
    //curs_set(1);
    wmove(rollwin, fieldheight - 2, 1);
    wclrtoeol(rollwin);
    waddch(rollwin, ':');
    waddstr(rollwin, expr.c_str());
    wmove(rollwin, fieldheight - 2, exprpos + 2);
    getyx(rollwin, cury, curx);
    box(rollwin, 0, 0);
    wrefresh(rollwin);
}

void Roller::listen(){
    int c;
    while((c = getch()) != 'q'){
        if(c != ERR){
            process(c);
        }
    }
}

bool Roller::move_up(){
    if(histpos > 0 && history.size() > 0){
        histpos--;
        expr = history[histpos];
        move_end();
        return true;
    }
    return false;
}

bool Roller::move_down(){
    if(histpos < history.size() && history.size() > 0){
        histpos++;
        if(histpos == history.size()){
            expr = "";
        }
        else{
            expr = history[histpos];
        }
        move_end();
        return true;
    }
    return false;
}

bool Roller::move_left(){
    if(exprpos > 0){
        exprpos--;
        return true;
    }
    return false;
}

bool Roller::move_right(){
    if(exprpos < expr.length()){
        exprpos++;
        return true;
    }
    return false;
}

bool Roller::move_home(){
    exprpos = 0;
    return true;
}

bool Roller::move_end(){
    exprpos = expr.length();
    return true;
}
int counter = 0;
void Roller::process(int c){
    switch(c){
        case KEY_UP:
            move_up();
            break;
        case KEY_DOWN:
            move_down();
            break;
        case KEY_END:
            move_end();
            break;
        case KEY_HOME:
            move_home();
            break;
        case KEY_LEFT:
            move_left();
            break;
        case KEY_RIGHT:
            move_right();
            break;
        case ' ':
        case 10:
        case 13:
        case KEY_ENTER:
            roll(expr);
            expr = "";
            move_home();
            histpos = history.size();
            break;
        case KEY_BACKSPACE:
            if(move_left()){
                update();
                remove();
            }
            break;
        case KEY_DC:
            remove();
            break;
        default:
            insert(c);
            break;
    }
    update();
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
