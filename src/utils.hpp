#pragma once

#include <vector>
#include <string>


template<class Element_type, class Container_type>
bool is_in_list(Element_type item, Container_type list){
    for(Element_type element : list){
        if (item == element) { return true; }
    }
    return false;
}

void tolower(std::string& s);
void toupper(std::string& s);
int max_length_in_list(std::vector<std::string>&);

struct coord{
    int x = 0;
    int y = 0;
};

struct WinPos{
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
};
