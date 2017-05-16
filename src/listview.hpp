#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define NORMAL 0
#define INSERT 1

typedef struct _win_st WINDOW;

class ListView{
    public:
        //void scroll(int);
        ListView();
        ListView(WINDOW*);
        ListView(WINDOW*, std::vector<std::string>);
        int numlines();
        int process(int);
        bool move_up();
        bool move_down();
        void update();
        void init();
        void listen();
        void insert(char);
        void remove();
        void wrap(int);
        ~ListView();
    private:
        bool created;
        int margin;
        int mode;
        int fieldheight;
        int fieldwidth;
        int scroll;
        int curx;
        int cury;
        std::vector<std::string> text;
        WINDOW* edwin;
};
