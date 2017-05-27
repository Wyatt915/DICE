#pragma once

//#include "sqlite_modern_cpp.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

typedef struct _win_st WINDOW;

struct fruit{
    std::string name;
    std::string desc;
};

//namespace sqlite {class database;}

class ListView{
    public:
        //void scroll(int);
        ListView(WINDOW*);
        ListView(WINDOW*, std::vector<fruit>);
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
        //sqlite::database* savefile;
        bool created;
        int margin;
        int mode;
        int fieldheight;
        int fieldwidth;
        int scroll;
        int curx;
        int cury;
        int selection;
        std::vector<fruit> text;
        WINDOW* lwin;
};
