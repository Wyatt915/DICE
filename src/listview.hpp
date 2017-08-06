#pragma once

#include "utils.hpp"

#include <string>

#define MTOP 0  //top margin
#define MRGT 1  //right margin
#define MBOT 2  //bottom margin
#define MLFT 3  //left margin

typedef struct _win_st WINDOW;
typedef struct panel PANEL;

class ListView : public DiceWin{
    public:
        ListView();
        ListView(WinPos);
        bool move_down();
        bool move_up();
        int numlines();
        //virtual void add_item() = 0;
        //virtual void edit_item() = 0;
        //virtual void init() = 0;
        void setFooter(std::string);
        void setHeader(std::string);
        void listen();
        virtual ~ListView();
    protected:
        int num_items;
        int selection;
};

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
