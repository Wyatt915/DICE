#include "basicinfo.hpp"
#include "characterdata.hpp"
#include "sqlite3.h"
#include "utils.hpp"

#include <ncurses.h>
#include <panel.h>
#include <string>

extern sqlite3* savefile;
extern gurps_cdata gchar_data;

BasicInfo::BasicInfo(){
}

BasicInfo::BasicInfo(WinPos dims):DiceWin(dims){
    box(win, 0, 0);
    display();
}

void BasicInfo::display(){
    int ypos = 1;
    int xpos = 1;
    std::string s;
    vstr_t maindisp = { "ST", "DX", "IQ", "HT", "HP", "WILL", "PER", "FP" };
    attribute a;
    for(int i = 0; i < 4; i++){
        a = gchar_data.attributes[maindisp[i]];
        s = maindisp[i] + ":";
        mvwaddstr(win, ypos, xpos, s.c_str());
        mvwaddstr(win, ypos, xpos + 4, std::to_string(a.level).c_str());
        ypos++;
    }

    ypos = 1;
    xpos = 12;

    for(int i = 4; i < 8; i++){
        a = gchar_data.attributes[maindisp[i]];
        s = maindisp[i] + ":";
        mvwaddstr(win, ypos, xpos, s.c_str());
        mvwaddstr(win, ypos, xpos + 6, std::to_string(a.level).c_str());
        if(maindisp[i] == "HP" || maindisp[i] == "FP"){
            s = "(" + std::to_string(a.currentvalue) + ")";
            mvwaddstr(win, ypos, xpos + 9, s.c_str());
        }
        ypos++;
    }

}

void BasicInfo::listen(){
    //do nothing
}

void BasicInfo::process(int i){
    //do nothing
}

void BasicInfo::update(){
    //do nothing
}
