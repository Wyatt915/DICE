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
    
   
}
