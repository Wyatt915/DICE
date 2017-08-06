/***************************************************************************************************
*                                                                                                  *
*                                        DICE source file                                          *
*   File:      list_skills.cpp                                                                     *
*   Author:    Wyatt Sheffield                                                                     *
*                                                                                                  *
*   Displays a verticall scroll window containing a list of all a character's skills, including    *
*   their difficulty and relative level.                                                           *
*                                                                                                  *
*                               Copyright (c) 2017 Wyatt Sheffield                                 *
*                                                                                                  *
***************************************************************************************************/

#include "characterdata.hpp"
#include "editor.hpp"
#include "list_skills.hpp"
#include "sqlite3.h"
#include "utils.hpp"
#include "parse.hpp"

#include <ctype.h>
#include <ncurses.h>
#include <panel.h>
#include <sstream>
#include <string.h>

extern sqlite3* savefile;
extern gurps_cdata gchar_data;

std::string find_rel_lvl(skill s){
    int level;
    int points = s.points;
    //gives the level with 1 point invested
    switch (s.diff){
        case 'E':
            level = 0;
            break;
        case 'A':
            level = -1;
            break;
        case 'H':
            level = -2;
            break;
        case 'V':
            level = -3;
            break;
    }
    
    if(points == 2 || points == 3){
        level += 1;
    }
    else if (points >= 4){
        level += 1 + points/4;
    }
    std::string out;
    if(level < 0){
        //If level is negative, we do not need to add a '-' sign
        out = s.base + std::to_string(level);
    }
    else{
        //if level is positive, we DO need to a dd a '+' sign.
        out = s.base + "+" + std::to_string(level);
    }
    return out;
}

std::string to_string(const skill& s){
    std::string out = "";
    out += s.name + ",";
    out += s.base + ",";
    out += s.diff + ",";
    out += s.desc + ",";
    out += std::to_string(s.points);
    return out;
}

//-------------------------------------[Constructor/Destructor]-------------------------------------

ListSkills::ListSkills(WinPos def):ListView(def){
    curs_set(0); //invisible cursor
    try{
        read_db();
    }
    catch(std::exception& e){
        mvaddstr(0, 0, e.what());
        throw e;
    }
    num_items = listitems.size();

    setTitle("Skills");
    std::string head = createHeader();
    setHeader(head);
    has_footer = false;

    curx = margin[MLFT];
    cury = margin[MTOP];
    update_panels();
    doupdate();
}

ListSkills::~ListSkills(){
}

std::string ListSkills::createHeader(){
    std::string h = "";
    std::string data[4] = {"Name", "Level", "Rel. Level", "Points"};
    //TODO dynamically calculate tabstops, based on the longest word in each field
    tabstops[0] = margin[MLFT];
    tabstops[1] = tabstops[0] + 16;
    tabstops[2] = tabstops[1] + 16;
    tabstops[3] = tabstops[2] + 16;
    for(int i = 0; i < 3; i++){
        h += data[i];
        while(h.length() - margin[MLFT] < tabstops[i + 1]){
            h += " ";
        }
    }
    h += data[3];
    return h;
}

//---------------------------------------[ncurses functions]---------------------------------------

void ListSkills::update(){
    wmove(win, 0, 0);
    wclrtobot(win);
    
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    
    if(has_header){
        mvwaddstr(win, margin[MLFT], 1, header.c_str());
    }
    int i;
    for(i = 0; i + scroll < num_items && i - scroll < fieldheight; i++){
        wmove(win, i + margin[MTOP] + scroll, margin[MLFT]);
        if(i + scroll == selection && has_focus){
            wattron(win, A_UNDERLINE);
            wattron(win, A_STANDOUT);
            waddstr(win, listitems[i + scroll].name.c_str());
            wattroff(win, A_UNDERLINE);
            wattroff(win, A_STANDOUT);

        }
        else{
            waddstr(win, listitems[i + scroll].name.c_str());
        }
        std::string abs_level = std::to_string(evaluate(find_rel_lvl(listitems[i+scroll])));
        wmove(win, i + margin[MTOP] + scroll, margin[MLFT] + tabstops[1] + 1);
        waddstr(win, abs_level.c_str());
        
        wmove(win, i + margin[MTOP] + scroll, margin[MLFT] + tabstops[2] + 1);
        waddstr(win, find_rel_lvl(listitems[i+scroll]).c_str());
        
        std::string invested = "[" + std::to_string(listitems[i + scroll].points) + "]";
        wmove(win, i + margin[MTOP] + scroll, margin[MLFT] + tabstops[3] + 1);
        waddstr(win, invested.c_str());
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

//----------------------------------------[Read/Write/Edit]----------------------------------------

static int read_db_callback(void* data, int argc, char** argv, char** azColName){
    skill temp;
    std::string diff;
    if(argv[0]) std::stringstream(argv[0]) >> temp.id;
    if(argv[1]) temp.name = std::string(argv[1]);
    if(argv[2]) std::stringstream(argv[2]) >> temp.base;
    if(argv[3]) temp.diff = argv[3][0];
    if(argv[4]) temp.desc = std::string(argv[4]);
    if(argv[5]) std::stringstream(argv[5]) >> temp.points;
    std::vector<skill>* skills  = reinterpret_cast<std::vector<skill>*>(data);
    skills->push_back(temp);
    return 0;
}

static int null_callback(void* notused, int argc, char** argv, char** azColName){
    return 0;
}
void ListSkills::read_db(){
    skill temp;
    const char* sql = "SELECT id,name,base,diff,description,points FROM skills";
    char* zErrMsg = 0;
    int rc = sqlite3_exec(savefile, sql, read_db_callback, (void*)&listitems, &zErrMsg);
    if(rc != SQLITE_OK){
        std::string errstr = "SQLite error: ";
        errstr += zErrMsg;
        sqlite3_free(zErrMsg);
        throw std::runtime_error(errstr);
    }
}

void ListSkills::add_item(){
    Editor e;
    skill newskill;
    e.setTitle("New Skill");
    e.show();
    e.listen();
    newskill.name = e.toString();
    if(newskill.name.length() > 0){
        e.clear();
        e.setTitle("Enter Description");
        e.listen();
        newskill.desc = e.toString();
        listitems.push_back(newskill);
        std::string sql = "INSERT INTO skills (name,base,diff,description,points) VALUES (";
        sql += to_string(newskill) + ");";
        char* zErrMsg = 0;
        int rc = sqlite3_exec(savefile, sql.c_str(), null_callback, NULL, &zErrMsg);
        if(rc != SQLITE_OK){
            std::string errstr = "SQLite error: ";
            errstr += zErrMsg;
            sqlite3_free(zErrMsg);
            e.hide();
            throw std::runtime_error(errstr);
        }
    }
    e.hide();
}

void ListSkills::remove_item(){

}

void ListSkills::edit_item(){
    Editor e;
    e.setTitle(listitems[selection].name);
    e.show();
    e.fill(listitems[selection].desc);
    e.update();
    e.listen();
    if(e.has_changed()){
        listitems[selection].desc = e.toString();
        std::string sql = "UPDATE skills SET description='" + listitems[selection].desc
            + "' WHERE id=" + std::to_string(listitems[selection].id) + ";";
        char* zErrMsg = 0;
        int rc = sqlite3_exec(savefile, sql.c_str(), null_callback, NULL, &zErrMsg);
        if(rc != SQLITE_OK){
            std::string errstr = "SQLite error: ";
            errstr += zErrMsg;
            sqlite3_free(zErrMsg);
            e.hide();
            throw std::runtime_error(errstr);
        }
    }
    e.hide();
}

void ListSkills::investPoints(int howmany){
    if(gchar_data.unspent_points - howmany < 0) { return; }
    gchar_data.unspent_points -= howmany;
    listitems[selection].points += howmany;
    std::string s = std::to_string(howmany);
    std::string id = std::to_string(listitems[selection].id);
    std::string sql = "BEGIN;";
    sql += "UPDATE skills SET points =  points + " + s + " WHERE id = " + id + ";";
    sql += "UPDATE character SET points = points - " + s + " WHERE id = 1;";
    sql += "COMMIT;";
    char* zErrMsg = 0;
    int rc = sqlite3_exec(savefile, sql.c_str(), null_callback, NULL, &zErrMsg);
    if(rc != SQLITE_OK){
        std::string errstr = "SQLite error: ";
        errstr += zErrMsg;
        sqlite3_free(zErrMsg);
        throw std::runtime_error(errstr);
    }
    update();
}

//--------------------------------------------[Process]--------------------------------------------

void ListSkills::process(int c){
    switch(c){
        case KEY_UP:
            move_up();
            break;
        case KEY_DOWN:
            move_down();
            break;
        case 'a':
            add_item();
            break;
        case 'x':
        case KEY_DC:
            remove_item();
            break;
        case '=':
        case '+':
            investPoints(1);
            break;
        case '-':
            investPoints(-1);
            break;
        case 10:
        case 13:
        case KEY_ENTER:
            edit_item();
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
