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

std::vector<std::string> disp_as_strvec(skill s){
    std::vector<std::string> out;
    out.push_back(s.name);
    out.push_back(std::to_string(evaluate(find_rel_lvl(s))));
    out.push_back(find_rel_lvl(s));
    out.push_back("[" + std::to_string(s.points) + "]");
    return out;
}

//-------------------------------------[Constructor/Destructor]-------------------------------------

ListSkills::ListSkills(WinPos def):ListView(def){
    try{
        read_db();
    }
    catch(std::exception& e){
        mvaddstr(0, 0, e.what());
        throw e;
    }
    num_items = skillvec.size();

    setTitle("Skills");
    std::string head = createHeader();
    setHeader(head);
    has_footer = false;
    
    for(skill s : skillvec){
        contents.push_back(disp_as_strvec(s));
    }

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
    std::vector<skill>* skills = reinterpret_cast<std::vector<skill>*>(data);
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
    int rc = sqlite3_exec(savefile, sql, read_db_callback, (void*)&skillvec, &zErrMsg);
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
        skillvec.push_back(newskill);
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
    e.setTitle(skillvec[selection].name);
    e.show();
    e.fill(skillvec[selection].desc);
    e.update();
    e.listen();
    if(e.has_changed()){
        skillvec[selection].desc = e.toString();
        std::string sql = "UPDATE skills SET description='" + skillvec[selection].desc
            + "' WHERE id=" + std::to_string(skillvec[selection].id) + ";";
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
    skillvec[selection].points += howmany;
    std::string s = std::to_string(howmany);
    std::string id = std::to_string(skillvec[selection].id);
    std::string sql = "BEGIN;";
    sql += "UPDATE skills SET points = points + " + s + " WHERE id = " + id + ";";
    sql += "UPDATE character SET unspentpoints = unspentpoints - " + s + " WHERE id = 1;";
    sql += "COMMIT;";
    char* zErrMsg = 0;
    int rc = sqlite3_exec(savefile, sql.c_str(), null_callback, NULL, &zErrMsg);
    if(rc != SQLITE_OK){
        std::string errstr = "SQLite error (Invest Points): ";
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
