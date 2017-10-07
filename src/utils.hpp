#pragma once

#include <vector>
#include <string>

typedef std::vector<std::string> vstr_t;
typedef std::vector<std::vector<std::string> > vvstr_t;

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

//wraps a vector of strings in another vector
vvstr_t vstr_to_vvstr(vstr_t);


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
