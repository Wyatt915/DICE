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
