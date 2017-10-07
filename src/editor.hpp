//editor.hpp
#pragma once

#include "utils.hpp"
#include "dicewin.hpp"
#include <string>
#include <vector>

#define NORMAL 0
#define INSERT 1
#define CHANGE 2

//we can't include <ncurses> here since the macros clash with some
//of the functions in std::algorithm.
typedef struct _win_st WINDOW;
typedef struct panel PANEL;

class Editor : public DiceWin{
    public:
        Editor();
        Editor(WinPos);
        Editor(WinPos, std::string);
        Editor(std::string);
        std::string toString(); //returns the text buffer as a single string.
        void process(int);   //interpret keyboard input
        bool move_up();     //Cursor movement functions
        bool move_down();
        bool move_left();
        bool move_right();
        bool move_home();
        bool move_end();
        bool has_changed(); //True if buffer has been edited
        void update();      //Refresh the window, etc.
        void init();        //Helper for the constructors
        void listen();      //wait for input
        void insert(char);  //insert into the text buffer
        void remove();      //delete char under the cursor
        void wrap(int row); //greedy word wrap algorithm
        void clear();       //clears the buffer
        void fill(std::string); //fill the buffer with the supplied string
        ~Editor();
    private:
        bool edited;
        int mode;
        std::vector<std::string> buffer;
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
