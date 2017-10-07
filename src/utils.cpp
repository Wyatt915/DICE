#include "utils.hpp"

void tolower(std::string& s){
    for(int i = 0; i < s.length(); i++){
        if(s[i] >= 'A' && s[i] <= 'Z'){
           s[i] = s[i] - 'A' + 'a';
        }
    }
}

void toupper(std::string& s){
    for(int i = 0; i < s.length(); i++){
        if(s[i] >= 'a' && s[i] <= 'z'){
           s[i] = s[i] - 'a' + 'A';
        }
    }
}

int max_length_in_list(std::vector<std::string>& l){
    int out = 0;
    for(std::string elem : l){
        if(elem.length() > out){ out = elem.length(); }
    }
    return out;
}

vvstr_t vstr_to_vvstr(vstr_t in){
    vvstr_t out;
    for(int i = 0; i < in.size(); i++){
        out.push_back(std::vector<std::string>(1,in[i]));
    }
    return out;
}

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
