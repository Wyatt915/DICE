#pragma once

#include "sqlite3.h"

#include <string>
#include <vector>

namespace gurps{

    struct skill{
        std::string name;
        std::string desc;
        int value;
        int points;
    };

    typedef skill adv;
    typedef skill dis;

    struct note{
        std::string title;
        std::string content;
        int priority = 0;
    };

    struct quest{
        std::string title;
        std::string givenBy;
        std::string desc;
        int reward;
        double timeframe;
    };
        
    struct disp{
        int points = 0;
        int st = 0;
        int dx = 0;
        int iq = 0;
        int ht = 0;
        int dmg = 0;
        int bl = 0;
        int hp = 0;
        int hp_current = 0;
        int will = 0;
        int fp = 0;
        int fp_current = 0;
        int speed = 0;
        int move = 0;
        std::vector<skill> skills;
        std::vector<adv> advantages;
        std::vector<dis> disadvantages;
        disp(int s, int d, int i, int h);
    };
    
    disp::disp(int s, int d, unt i, int h){
        st = s; dx = d; iq = i; ht = h;
        dmg = st;
        bl = st * st / 5;
        will = iq;

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
