/***************************************************************************************************
*                                                                                                  *
*                                        DICE source file                                          *
*   File:      list_items.cpp                                                                      *
*   Author:    Wyatt Sheffield                                                                     *
*                                                                                                  *
*   Defines a vertically scrolling window that displays the character's inventory, excluding       *
*   weapons and armor. The player may use this window to buy and sell items, view an item's        *
*   lore or description, and add or create new items.                                              *
*                                                                                                  *
*                               Copyright (c) 2017 Wyatt Sheffield                                 *
*                                                                                                  *
***************************************************************************************************/

#include "characterdata.hpp"
#include "editor.hpp"
#include "list_items.hpp"
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

//-------------------------------------[Constructor/Destructor]-------------------------------------

ListItems::ListItems(WinPos def):ListView(def){
    curs_set(0); //invisible cursor
    try{
        read_db();
    }
    catch(std::exception& e){
        throw e;
    }
    num_items = inventory.size();

    setTitle("Skills");
    std::string head = createHeader();
    setHeader(head);
    has_footer = false;

    curx = margin[MLFT];
    cury = margin[MTOP];
    update_panels();
    doupdate();
}

ListItems::~ListItems(){
}

std::string ListItems::createHeader(){
    std::string h = "";
    std::string data[4] = {"Name", "Weight", "Cost", "Qty"};
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

void ListItems::update(){
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
            waddstr(win, inventory[i + scroll].name.c_str());
            wattroff(win, A_UNDERLINE);
            wattroff(win, A_STANDOUT);

        }
        else{
            waddstr(win, inventory[i + scroll].name.c_str());
        }
        
        std::string weight = std::to_string(inventory[i + scroll].weight);
        std::string cost = std::to_string(inventory[i + scroll].cost);
        std::string quantity = std::to_string(inventory[i + scroll].quantity);
        
        wmove(win, i + margin[MTOP] + scroll, margin[MLFT] + tabstops[1] + 1);
        waddstr(win, weight.c_str());
        
        wmove(win, i + margin[MTOP] + scroll, margin[MLFT] + tabstops[2] + 1);
        waddstr(win, cost.c_str());
        
        wmove(win, i + margin[MTOP] + scroll, margin[MLFT] + tabstops[3] + 1);
        waddstr(win, quantity.c_str());
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
    item temp;
    if(argv[0]) std::stringstream(argv[0]) >> temp.id;
    if(argv[1]) temp.name = argv[1];
    if(argv[2]) std::stringstream(argv[2]) >> temp.weight;
    if(argv[3]) std::stringstream(argv[3]) >> temp.cost;
    if(argv[4]) std::stringstream(argv[4]) >> temp.quantity;
    if(argv[5]) temp.desc = argv[5];
    if(argv[6]) temp.lore = argv[6];
    std::vector<item>* items  = reinterpret_cast<std::vector<item>*>(data);
    items->push_back(temp);
    return 0;
}

static int null_callback(void* notused, int argc, char** argv, char** azColName){
    return 0;
}

void ListItems::read_db(){
    item temp;
    const char* sql = "SELECT id,name,weight,cost,quantity,description,lore FROM inventory";
    char* zErrMsg = 0;
    int rc = sqlite3_exec(savefile, sql, read_db_callback, (void*)&inventory, &zErrMsg);
    if(rc != SQLITE_OK){
        std::string errstr = "SQLite error: ";
        errstr += zErrMsg;
        sqlite3_free(zErrMsg);
        throw std::runtime_error(errstr);
    }
}

void ListItems::add_item(){
//    Editor e;
//    skill newskill;
//    e.setTitle("New Skill");
//    e.show();
//    e.listen();
//    newskill.name = e.toString();
//    if(newskill.name.length() > 0){
//        e.clear();
//        e.setTitle("Enter Description");
//        e.listen();
//        newskill.desc = e.toString();
//        inventory.push_back(newskill);
//        std::string sql = "INSERT INTO skills (name,base,diff,description,points) VALUES (";
//        sql += to_string(newskill) + ");";
//        char* zErrMsg = 0;
//        int rc = sqlite3_exec(savefile, sql.c_str(), null_callback, NULL, &zErrMsg);
//        if(rc != SQLITE_OK){
//            std::string errstr = "SQLite error: ";
//            errstr += zErrMsg;
//            sqlite3_free(zErrMsg);
//            e.hide();
//            throw std::runtime_error(errstr);
//        }
//    }
//    e.hide();
}

void ListItems::remove_item(){

}

void ListItems::edit_item(){
//    Editor e;
//    e.setTitle(inventory[selection].name);
//    e.show();
//    e.fill(inventory[selection].desc);
//    e.update();
//    e.listen();
//    if(e.has_changed()){
//        inventory[selection].desc = e.toString();
//        std::string sql = "UPDATE skills SET description='" + inventory[selection].desc
//            + "' WHERE id=" + std::to_string(inventory[selection].id) + ";";
//        char* zErrMsg = 0;
//        int rc = sqlite3_exec(savefile, sql.c_str(), null_callback, NULL, &zErrMsg);
//        if(rc != SQLITE_OK){
//            std::string errstr = "SQLite error: ";
//            errstr += zErrMsg;
//            sqlite3_free(zErrMsg);
//            e.hide();
//            throw std::runtime_error(errstr);
//        }
//    }
//    e.hide();
}

void ListItems::investPoints(int howmany){
//    if(gchar_data.unspent_points - howmany < 0) { return; }
//    gchar_data.unspent_points -= howmany;
//    inventory[selection].points += howmany;
//    std::string s = std::to_string(howmany);
//    std::string id = std::to_string(inventory[selection].id);
//    std::string sql = "BEGIN;";
//    sql += "UPDATE skills SET points =  points + " + s + " WHERE id = " + id + ";";
//    sql += "UPDATE character SET points = points - " + s + " WHERE id = 1;";
//    sql += "COMMIT;";
//    char* zErrMsg = 0;
//    int rc = sqlite3_exec(savefile, sql.c_str(), null_callback, NULL, &zErrMsg);
//    if(rc != SQLITE_OK){
//        std::string errstr = "SQLite error: ";
//        errstr += zErrMsg;
//        sqlite3_free(zErrMsg);
//        throw std::runtime_error(errstr);
//    }
//    update();
}

//--------------------------------------------[Process]--------------------------------------------

int ListItems::process(int c){
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
