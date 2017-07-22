//dicewindow.hpp

#pragma once

#include "utils.hpp"

#include <string>

#define MTOP 0  //top margin
#define MRGT 1  //right margin
#define MBOT 2  //bottom margin
#define MLFT 3  //left margin

typedef struct _win_st WINDOW;
typedef struct panel PANEL;

class DiceWin{
    public:
        DiceWin();
        DiceWin(WinPos);
        //virtual int process(int) = 0;
        void setFooter(std::string);
        void setHeader(std::string);
        void setTitle(std::string);
        void show();
        void hide();
        void give_focus();
        void revoke_focus();
        virtual ~DiceWin();
    protected:
        bool has_footer;
        bool has_focus;
        bool has_header;
        int curx;
        int cury;
        int fieldheight;
        int fieldwidth;
        int margin[4]; //{top, right, bottom, left}
        int scroll;
        std::string header;
        std::string footer;
        std::string title;
        WINDOW* win;
        PANEL* pan;
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
