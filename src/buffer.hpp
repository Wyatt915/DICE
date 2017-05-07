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
        int find_pos_in_raw(int, int);
        void get_coord_from_pos(int&, int&, int);
        const char* debug();
    private:
        int margin;
        int columns;
        std::string raw;
        std::string debuginfo;
        std::vector<std::string> disp;
};

