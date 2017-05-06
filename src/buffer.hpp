#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define BUFF_TRIV 0
#define BUFF_CRLF 1
#define BUFF_WRAP 2

class Buffer{
    public:
        friend std::ostream& operator<<(std::ostream& os, Buffer& b);
        int insert(char, int&, int&); //row, column
        void update();
        int remove(int&, int&);
        std::string to_string();
        Buffer();
        Buffer(int);
        Buffer(std::string);
        Buffer(std::string, int);
        const char* getline(int);
        int numlines();
        int getlinelen(int);
    private:
        int xOff;
        int yOff;
        int columns;
        int lines;
        //int effrow();
        //int effcol();
        std::vector<std::string> data;
};

