#pragma once

#include <string>
#include <vector>

#define NORMAL 0
#define INSERT 1
#define CHANGE 2

//we can't include <ncurses> here since the macros clash with some
//of the functions in std::algorithm.
typedef struct _win_st WINDOW;

class Editor{
    public:
        Editor();
        Editor(WINDOW*);
        Editor(WINDOW*, std::string);
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
        ~Editor();
    private:
        bool created;
        bool edited;
        int margin;
        int mode;
        int fieldheight;
        int fieldwidth;
        int scroll;
        int curx;
        int cury;
        std::vector<std::string> buffer;
        WINDOW* edwin;
};
