
#include "characterdata.hpp"
#include "editor.hpp"
#include "list_adv.hpp"
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

vstr_t disp_as_strvec(const advantage& a){
    vstr_t out;
    out.push_back(a.name);
    out.push_back(std::to_string(a.cost));
    return out;
}

//-------------------------------------[Constructor/Destructor]-------------------------------------

ListAdv::ListAdv(WinPos def):ListView(def){
    try{
        read_db();
    }
    catch(std::exception& e){
        mvaddstr(0, 0, e.what());
        throw e;
    }
    num_items = advantages.size();

    setTitle("Skills");
    std::string head = createHeader();
    setHeader(head);
    has_footer = false;
    
    for(advantage a : advantages){
        contents.push_back(disp_as_strvec(a));
    }

    curx = margin[MLFT];
    cury = margin[MTOP];
    update_panels();
    doupdate();
}

ListAdv::~ListAdv(){
}

std::string ListAdv::createHeader(){
    std::string h = "";
    std::string data[2] = {"Name",  "Points"};
    //TODO dynamically calculate tabstops, based on the longest word in each field
    tabstops[0] = margin[MLFT];
    tabstops[1] = tabstops[0] + 48;
    //tabstops[2] = tabstops[1] + 16;
    //tabstops[3] = tabstops[2] + 16;
    for(int i = 0; i < 1; i++){
        h += data[i];
        while(h.length() - margin[MLFT] < tabstops[i + 1]){
            h += " ";
        }
    }
    h += data[1];
    return h;
}


//----------------------------------------[Read/Write/Edit]----------------------------------------

static int read_db_callback(void* data, int argc, char** argv, char** azColName){
    advantage temp;
    std::string diff;
    if(argv[0]) std::stringstream(argv[0]) >> temp.id;
    if(argv[1]) temp.name = std::string(argv[1]);
    if(argv[2]) std::stringstream(argv[2]) >> temp.min;
    if(argv[3]) std::stringstream(argv[3]) >> temp.cost;
    if(argv[4]) std::stringstream(argv[4]) >> temp.max;
    if(argv[5]) temp.desc = std::string(argv[5]);
    if(argv[6]) std::stringstream(argv[6]) >> temp.points;
    std::vector<advantage>* adv = reinterpret_cast<std::vector<advantage>*>(data);
    adv->push_back(temp);
    return 0;
}

static int null_callback(void* notused, int argc, char** argv, char** azColName){
    return 0;
}
void ListAdv::read_db(){
    const char* sql = "SELECT id,name,minimum,cost,maxlevel,description,points FROM advantages";
    char* zErrMsg = 0;
    int rc = sqlite3_exec(savefile, sql, read_db_callback, (void*)&advantages, &zErrMsg);
    if(rc != SQLITE_OK){
        std::string errstr = "SQLite error while loading advantages: ";
        errstr += zErrMsg;
        sqlite3_free(zErrMsg);
        throw std::runtime_error(errstr);
    }
}

void ListAdv::add_item(){
    //Editor e;
    //skill newskill;
    //e.setTitle("New Skill");
    //e.show();
    //e.listen();
    //newskill.name = e.toString();
    //if(newskill.name.length() > 0){
    //    e.clear();
    //    e.setTitle("Enter Description");
    //    e.listen();
    //    newskill.desc = e.toString();
    //    advantages.push_back(newskill);
    //    std::string sql = "INSERT INTO skills (name,base,diff,description,points) VALUES (";
    //    sql += to_string(newskill) + ");";
    //    char* zErrMsg = 0;
    //    int rc = sqlite3_exec(savefile, sql.c_str(), null_callback, NULL, &zErrMsg);
    //    if(rc != SQLITE_OK){
    //        std::string errstr = "SQLite error: ";
    //        errstr += zErrMsg;
    //        sqlite3_free(zErrMsg);
    //        e.hide();
    //        throw std::runtime_error(errstr);
    //    }
    //}
    //e.hide();
}

void ListAdv::remove_item(){

}

void ListAdv::edit_item(){
    //Editor e;
    //e.setTitle(advantages[selection].name);
    //e.show();
    //e.fill(advantages[selection].desc);
    //e.update();
    //e.listen();
    //if(e.has_changed()){
    //    advantages[selection].desc = e.toString();
    //    std::string sql = "UPDATE skills SET description='" + advantages[selection].desc
    //        + "' WHERE id=" + std::to_string(advantages[selection].id) + ";";
    //    char* zErrMsg = 0;
    //    int rc = sqlite3_exec(savefile, sql.c_str(), null_callback, NULL, &zErrMsg);
    //    if(rc != SQLITE_OK){
    //        std::string errstr = "SQLite error: ";
    //        errstr += zErrMsg;
    //        sqlite3_free(zErrMsg);
    //        e.hide();
    //        throw std::runtime_error(errstr);
    //    }
    //}
    //e.hide();
}

void ListAdv::investPoints(int howmany){
    //if(gchar_data.unspent_points - howmany < 0) { return; }
    //gchar_data.unspent_points -= howmany;
    //advantages[selection].points += howmany;
    //std::string s = std::to_string(howmany);
    //std::string id = std::to_string(advantages[selection].id);
    //std::string sql = "BEGIN;";
    //sql += "UPDATE skills SET points = points + " + s + " WHERE id = " + id + ";";
    //sql += "UPDATE character SET unspentpoints = unspentpoints - " + s + " WHERE id = 1;";
    //sql += "COMMIT;";
    //char* zErrMsg = 0;
    //int rc = sqlite3_exec(savefile, sql.c_str(), null_callback, NULL, &zErrMsg);
    //if(rc != SQLITE_OK){
    //    std::string errstr = "SQLite error (Invest Points): ";
    //    errstr += zErrMsg;
    //    sqlite3_free(zErrMsg);
    //    throw std::runtime_error(errstr);
    //}
    //update();
}

//--------------------------------------------[Process]--------------------------------------------

void ListAdv::process(int c){
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
