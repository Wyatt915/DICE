//characterdata.hpp
#pragma once
#include <string>
#include <map>

struct attribute{
    std::string name;
    std::string base;
    bool primary;
    int level;
    int points;
    int cost;
};

typedef struct gurps_cdata{
    std::string name;
    int total_points, unspent_points;
    int starting_cash, current_cash;
    std::map<std::string, attribute> attributes;
    int current_fp;
    int current_hp;
    std::string thr, sw;
    int basic_speed, basic_lift, basic_move;
}gurps_cdata;

extern gurps_cdata character_data;

void read_cdata(); //read in from the savefile

void write_cdata(); //write to savefile
