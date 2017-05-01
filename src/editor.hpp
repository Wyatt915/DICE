#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <ncurses.h>

#define NORMAL 0
#define INSERT 1
#define ED_MOV 2
#define ED_INS 3
#define ED_DEL 4

class Buffer;

class Editor{
    public:
        //void scroll(int);
        Editor(WINDOW*);
        Editor(WINDOW*, std::string);
        const char* getline(int);
        int numlines();
        std::string disp();
        int process(int);
        ~Editor();
    private:
        int mode;
        int columns;
        int lines;
        Buffer* data;
        WINDOW* edwin;
};
