//characterdata.cpp

#include "characterdata.hpp"
#include "sqlite3.h"
#include <sstream>

extern sqlite3* savefile;

std::string[100][2] damagetable = {
    {"1d-6",  "1d-5"},   {"1d-6",  "1d-5"},   {"1d-5",  "1d-4"},   {"1d-5",  "1d-4"},
    {"1d-4",  "1d-3"},   {"1d-4",  "1d-3"},   {"1d-3",  "1d-2"},   {"1d-3",  "1d-2"},
    {"1d-2",  "1d-1"},   {"1d-2",  "1d"},     {"1d-1",  "1d+1"},   {"1d-1",  "1d+2"},
    {"1d",    "2d-1"},   {"1d",    "2d"},     {"1d+1",  "2d+1"},   {"1d+1",  "2d+2"},
    {"1d+2",  "3d-1"},   {"1d+2",  "3d"},     {"2d-1",  "3d+1"},   {"2d-1",  "3d+2"},
    {"2d",    "4d-1"},   {"2d",    "4d"},     {"2d+1",  "4d+1"},   {"2d+1",  "4d+2"},
    {"2d+2",  "5d-1"},   {"2d+2",  "5d"},     {"3d-1",  "5d+1"},   {"3d-1",  "5d+1"},
    {"3d",    "5d+2"},   {"3d",    "5d+2"},   {"3d+1",  "6d-1"},   {"3d+1",  "6d-1"},
    {"3d+2",  "6d"},     {"3d+2",  "6d"},     {"4d-1",  "6d+1"},   {"4d-1",  "6d+1"},
    {"4d",    "6d+2"},   {"4d",    "6d+2"},   {"4d+1",  "7d-1"},   {"4d+1",  "7d-1"},
    {"4d+1",  "7d-1"},   {"4d+1",  "7d-1"},   {"4d+1",  "7d-1"},   {"4d+1",  "7d-1"},
    {"5d",    "7d+1"},   {"5d",    "7d+1"},   {"5d",    "7d+1"},   {"5d",    "7d+1"},
    {"5d",    "7d+1"},   {"5d+2",  "8d-1"},   {"5d+2",  "8d-1"},   {"5d+2",  "8d-1"},
    {"5d+2",  "8d-1"},   {"5d+2",  "8d-1"},   {"6d",    "8d+1"},   {"6d",    "8d+1"},
    {"6d",    "8d+1"},   {"6d",    "8d+1"},   {"6d",    "8d+1"},   {"7d-1",  "9d"},
    {"7d-1",  "9d"},     {"7d-1",  "9d"},     {"7d-1",  "9d"},     {"7d-1",  "9d"},
    {"7d+1",  "9d+2"},   {"7d+1",  "9d+2"},   {"7d+1",  "9d+2"},   {"7d+1",  "9d+2"},
    {"7d+1",  "9d+2"},   {"8d",    "10d"},    {"8d",    "10d"},    {"8d",    "10d"},
    {"8d",    "10d"},    {"8d",    "10d"},    {"8d+2",  "10d+2"},  {"8d+2",  "10d+2"},
    {"8d+2",  "10d+2"},  {"8d+2",  "10d+2"},  {"8d+2",  "10d+2"},  {"9d",    "11d"},
    {"9d",    "11d"},    {"9d",    "11d"},    {"9d",    "11d"},    {"9d",    "11d"},
    {"9d+2",  "11d+2"},  {"9d+2",  "11d+2"},  {"9d+2",  "11d+2"},  {"9d+2",  "11d+2"},
    {"9d+2",  "11d+2"},  {"10d",   "12d"},    {"10d",   "12d"},    {"10d",   "12d"},
    {"10d",   "12d"},    {"10d",   "12d"},    {"10d+2", "12d+2"},  {"10d+2", "12d+2"},
    {"10d+2", "12d+2"},  {"10d+2", "12d+2"},  {"10d+2", "12d+2"},  {"11d",   "13d"}
};

static int read_attributes(void* notused, int argc, char** argv, char ** azColName)
{
    attribute temp;
    std::string abbr(argv[0]);
    temp.name = argv[1];
    temp.primary = true;
    std::stringstream(argv[2]) >> temp.level;
    std::stringstream(argv[3]) >> temp.cost;
    std::stringstream(argv[4]) >> temp.points;
    character_data.attributes[abbr] = temp;
    return 0;
}

static int write_callback(void* notused, int argc, char** argv, char ** azColName)
{
    return 0;
}

void read_cdata(){
    std::string sql = "SELECT abbr,name,lvl,cost,invested FROM attributes;";
    char* zErrMsg = nullptr;
    int rc = sqlite3_exec(savefile, sql.c_str(), read_attributes, NULL, &zErrMsg);
    if(rc != SQLITE_OK){
        std::string s = zErrMsg;
        sqlite3_free(zErrMsg);
        throw std::runtime_error(s);
    }
    sql = "
