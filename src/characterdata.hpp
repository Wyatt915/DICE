/***************************************************************************************************
*                                                                                                  *
*                                        DICE source file                                          *
*   File:      characterdata.hpp                                                                   *
*   Author:    Wyatt Sheffield                                                                     *
*                                                                                                  *
*   Defines structures to hold basic global data for the character.                                *
*                                                                                                  *
*                               Copyright (c) 2017 Wyatt Sheffield                                 *
*                                                                                                  *
***************************************************************************************************/

#pragma once
#include <string>
#include <map>

struct attribute{
    std::string name = "";
    std::string base = "";
    bool primary = true;
    int level = 0;
    int currentvalue = 0;
    int points = 0;
    int cost = 1; //We divide by this value, so make it non zero by default
};

typedef struct gurps_cdata{
    std::string name = "";
    int total_points = 0;
    int unspent_points = 0;
    int starting_cash = 0;
    int current_cash = 0;
    std::map<std::string, attribute> attributes;
    std::string thrust = "";
    std::string swing = "";
}gurps_cdata;


void load_cdata(); //read in from the savefile

void save_cdata(); //write to savefile
