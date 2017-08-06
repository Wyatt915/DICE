/***************************************************************************************************
*                                                                                                  *
*                                        DICE source file                                          *
*   File:      characterdata.cpp                                                                   *
*   Author:    Wyatt Sheffield                                                                     *
*                                                                                                  *
*   Defines structures to hold basic global data for the character.                                *
*                                                                                                  *
*                               Copyright (c) 2017 Wyatt Sheffield                                 *
*                                                                                                  *
***************************************************************************************************/

#include "characterdata.hpp"
#include "sqlite3.h"
#include <sstream>
#include <stdexcept>
#include "parse.hpp"

extern sqlite3* savefile;
extern gurps_cdata gchar_data;

// (thrust, swing)
const std::string damagetable[100][2] = {
    {"1d6-6",  "1d6-5"},   {"1d6-6",  "1d6-5"},   {"1d6-5",  "1d6-4"},   {"1d6-5",  "1d6-4"},
    {"1d6-4",  "1d6-3"},   {"1d6-4",  "1d6-3"},   {"1d6-3",  "1d6-2"},   {"1d6-3",  "1d6-2"},
    {"1d6-2",  "1d6-1"},   {"1d6-2",  "1d6"},     {"1d6-1",  "1d6+1"},   {"1d6-1",  "1d6+2"},
    {"1d6",    "2d6-1"},   {"1d6",    "2d6"},     {"1d6+1",  "2d6+1"},   {"1d6+1",  "2d6+2"},
    {"1d6+2",  "3d6-1"},   {"1d6+2",  "3d6"},     {"2d6-1",  "3d6+1"},   {"2d6-1",  "3d6+2"},
    {"2d6",    "4d6-1"},   {"2d6",    "4d6"},     {"2d6+1",  "4d6+1"},   {"2d6+1",  "4d6+2"},
    {"2d6+2",  "5d6-1"},   {"2d6+2",  "5d6"},     {"3d6-1",  "5d6+1"},   {"3d6-1",  "5d6+1"},
    {"3d6",    "5d6+2"},   {"3d6",    "5d6+2"},   {"3d6+1",  "6d6-1"},   {"3d6+1",  "6d6-1"},
    {"3d6+2",  "6d6"},     {"3d6+2",  "6d6"},     {"4d6-1",  "6d6+1"},   {"4d6-1",  "6d6+1"},
    {"4d6",    "6d6+2"},   {"4d6",    "6d6+2"},   {"4d6+1",  "7d6-1"},   {"4d6+1",  "7d6-1"},
    {"4d6+1",  "7d6-1"},   {"4d6+1",  "7d6-1"},   {"4d6+1",  "7d6-1"},   {"4d6+1",  "7d6-1"},
    {"5d6",    "7d6+1"},   {"5d6",    "7d6+1"},   {"5d6",    "7d6+1"},   {"5d6",    "7d6+1"},
    {"5d6",    "7d6+1"},   {"5d6+2",  "8d6-1"},   {"5d6+2",  "8d6-1"},   {"5d6+2",  "8d6-1"},
    {"5d6+2",  "8d6-1"},   {"5d6+2",  "8d6-1"},   {"6d6",    "8d6+1"},   {"6d6",    "8d6+1"},
    {"6d6",    "8d6+1"},   {"6d6",    "8d6+1"},   {"6d6",    "8d6+1"},   {"7d6-1",  "9d6"},
    {"7d6-1",  "9d6"},     {"7d6-1",  "9d6"},     {"7d6-1",  "9d6"},     {"7d6-1",  "9d6"},
    {"7d6+1",  "9d6+2"},   {"7d6+1",  "9d6+2"},   {"7d6+1",  "9d6+2"},   {"7d6+1",  "9d6+2"},
    {"7d6+1",  "9d6+2"},   {"8d6",    "10d6"},    {"8d6",    "10d6"},    {"8d6",    "10d6"},
    {"8d6",    "10d6"},    {"8d6",    "10d6"},    {"8d6+2",  "10d6+2"},  {"8d6+2",  "10d6+2"},
    {"8d6+2",  "10d6+2"},  {"8d6+2",  "10d6+2"},  {"8d6+2",  "10d6+2"},  {"9d6",    "11d6"},
    {"9d6",    "11d6"},    {"9d6",    "11d6"},    {"9d6",    "11d6"},    {"9d6",    "11d6"},
    {"9d6+2",  "11d6+2"},  {"9d6+2",  "11d6+2"},  {"9d6+2",  "11d6+2"},  {"9d6+2",  "11d6+2"},
    {"9d6+2",  "11d6+2"},  {"10d6",   "12d6"},    {"10d6",   "12d6"},    {"10d6",   "12d6"},
    {"10d6",   "12d6"},    {"10d6",   "12d6"},    {"10d6+2", "12d6+2"},  {"10d6+2", "12d6+2"},
    {"10d6+2", "12d6+2"},  {"10d6+2", "12d6+2"},  {"10d6+2", "12d6+2"},  {"11d6",   "13d6"}
};

static int read_attributes(void* notused, int argc, char** argv, char ** azColName)
{
    attribute temp;
    temp.primary = true;
    std::string abbr;
    if(argv[0]) abbr = argv[0];
    if(argv[1]) temp.name = argv[1];
    if(argv[2]) std::stringstream(argv[2]) >> temp.level;
    if(argv[3]) std::stringstream(argv[3]) >> temp.cost;
    if(argv[4]) std::stringstream(argv[4]) >> temp.points;
    gchar_data.attributes[abbr] = temp;
    return 0;
}

static int read_secondary(void* notused, int argc, char** argv, char ** azColName)
{
    attribute temp;
    temp.primary = false;
    std::string abbr;
    if(argv[0]) abbr = argv[0];
    if(argv[1]) temp.name = argv[1];
    if(argv[2]) std::stringstream(argv[2]) >> temp.cost;
    if(argv[3]) std::stringstream(argv[3]) >> temp.points;
    if(argv[4]) temp.base = argv[4];
    if(argv[5]) std::stringstream(argv[5]) >> temp.currentvalue;
    gchar_data.attributes[abbr] = temp;
    return 0;
}

static int read_character(void* notused, int argc, char** argv, char ** azColName)
{
    if(argv[0]) gchar_data.name = argv[0];
    if(argv[1]) std::stringstream(argv[1]) >> gchar_data.unspent_points;
    if(argv[2]) std::stringstream(argv[2]) >> gchar_data.starting_cash;
    return 0;
}

static int write_callback(void* notused, int argc, char** argv, char ** azColName)
{
    return 0;
}

void load_cdata(){
    char* zErrMsg = nullptr;
    
    std::string errmsg = "SQLite error on reading character data: ";

    std::string sql = "SELECT abbr,name,lvl,cost,points FROM attributes;";
    int rc = sqlite3_exec(savefile, sql.c_str(), read_attributes, NULL, &zErrMsg);
    if(rc != SQLITE_OK){
        errmsg += zErrMsg;
        sqlite3_free(zErrMsg);
        throw std::runtime_error(errmsg);
    }
    
    sql = "SELECT abbr,name,cost,points,base,currentvalue FROM secondary;";
    rc = sqlite3_exec(savefile, sql.c_str(), read_secondary, NULL, &zErrMsg);
    if(rc != SQLITE_OK){
        errmsg += zErrMsg;
        sqlite3_free(zErrMsg);
        throw std::runtime_error(errmsg);
    }
    
    sql = "SELECT name,unspentpoints,startcash FROM character;";
    rc = sqlite3_exec(savefile, sql.c_str(), read_character, NULL, &zErrMsg);
    if(rc != SQLITE_OK){
        errmsg += zErrMsg;
        sqlite3_free(zErrMsg);
        throw std::runtime_error(errmsg);
    }

    int st = gchar_data.attributes["ST"].level;
    st = st <= 100 ? st : 100;
    gchar_data.swing = damagetable[st-1][0];
    gchar_data.thrust = damagetable[st-1][1];
    
    for(auto i = std::begin(gchar_data.attributes); i != std::end(gchar_data.attributes); ++i){
        //if it is a secondary attribute, we need to calculate the level.
        if(!i->second.primary){
            int l = evaluate(i->second.base);
            l += i->second.points / i->second.cost;
            i->second.level = l;
        }
    }
}
