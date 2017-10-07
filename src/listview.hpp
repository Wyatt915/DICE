#pragma once

#include "utils.hpp"

#include <string>

#define MTOP 0  //top margin
#define MRGT 1  //right margin
#define MBOT 2  //bottom margin
#define MLFT 3  //left margin

class ListView : public DiceWin{
    public:
        ListView();
        ListView(WinPos);
        bool move_down();
        bool move_up();
        int numlines();
        void process(int);
        //virtual void add_item() = 0;
        //virtual void edit_item() = 0;
        //virtual void init() = 0;
        void setContents(vvstr_t);
        void setFooter(std::string);
        void setHeader(std::string);
        vstr_t getSelection(int&);
        void listen();
        void update();
        virtual ~ListView();
    protected:
        int num_items;
        int selection;
        int tabstops[4];
        vvstr_t contents;
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
