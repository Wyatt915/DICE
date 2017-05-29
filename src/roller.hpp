#pragma once

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
        ~Roller();
    private:
        bool created;
        bool edited;
        int margin;
        int mode;
        int fieldheight;
        int fieldwidth;
        int curx;
        int cury;
        std::vector<std::string> buffer;
        std::string title;
        WINDOW* rollwin;
        PANEL* rollpanel;        
};
