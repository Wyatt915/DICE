#pragma once

#include "parse.hpp"

#include <string>
#include <vector>

//we can't include <ncurses> here since the macros clash with some
//of the functions in std::algorithm.
typedef struct _win_st WINDOW;
typedef struct panel PANEL;


class Roller{
    public:
        Roller();
        Roller(WINDOW*);
        Roller(WINDOW*, std::string);
        Roller(std::string);
        bool move_up();
        bool move_down();
        bool move_left();
        bool move_right();
        bool move_end();
        bool move_home();
        void process(int);  //interpret keyboard input
        void update();      //Refresh the window, etc.
        void init();        //Helper for the constructors
        void listen();      //wait for input
        void insert(char);  //insert into the text buffer
        void remove();      //delete char under the cursor
        void show();
        void hide();
        void setTitle(std::string);
        void roll(std::string);
        //void roll(SyntaxTree);
        void display(int);
        ~Roller();
    private:
        int prev_cursor_vis;
        bool created;
        bool edited;
        int margin;
        int mode;
        int fieldheight;
        int fieldwidth;
        int curx;
        int cury;
        int exprpos;
        std::vector<std::string> history;
        int histpos;
        std::string title;
        std::string expr;
        SyntaxTree prevexpr;
        WINDOW* rollwin;
        PANEL* rollpanel;
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
