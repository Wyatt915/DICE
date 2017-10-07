#pragma once

#include "dicewin.hpp"
#include "utils.hpp"

class BasicInfo : public DiceWin{
    public:
        BasicInfo();
        BasicInfo(WinPos);
        void display();
        void listen();
        void process(int);
        void update();
};
