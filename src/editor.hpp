#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define NORMAL 0
#define INSERT 1
#define ED_MOV 2
#define ED_INS 3
#define ED_DEL 4

class Buffer;
typedef struct _win_st WINDOW;

class Editor{
    public:
        //void scroll(int);
        Editor();
        Editor(WINDOW*);
        Editor(WINDOW*, std::string);
        Editor(std::string);
        std::string toString();
        int process(int);
        bool move_up();
        bool move_down();
        bool move_left();
        bool move_right();
        bool move_home();
        bool move_end();
        void update();
        void init();
        void listen();
        void insert(char);
        void remove();
        void wrap(int);
        ~Editor();
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
