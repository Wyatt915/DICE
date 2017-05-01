#include "buffer.hpp"
#include <algorithm>

void flow(std::vector<std::string>& l,std::string in, int columns){
    std::string::iterator wspos = in.begin(); //position of last whitespace
    std::string::iterator it = in.begin();
    std::string::iterator first = in.begin();
    int d;
    while(it != in.end()){
        d = std::distance(first, it);
        if(d > columns){
            l.push_back(std::string(first, wspos)); //TODO: Handle cases where a word may be longer than the number of columns
            first = wspos;
            first++;
            it = first;
            continue;
        }
        else if(*it == ' '){ wspos = it; }
        it++;
    }
    l.push_back(std::string(first, it)); 
}



Buffer::Buffer(){
    data.push_back("");
    columns = 80;
    lines = 1;
}

Buffer::Buffer(int c) : columns(c){
    data.push_back("");
    lines = 1;
}

Buffer::Buffer(std::string in){
    columns = 80;
    flow(data, in, columns);
    lines = data.size();
}

Buffer::Buffer(std::string in, int c) : columns(c) {
    flow(data, in, columns);
    lines = data.size();
}



std::string Buffer::to_string(){
    std::string out = "";
    for(std::string l : data){
        out += l + "\n\r";
    }
    return out;
}

const char* Buffer::getline(int l){
    return data[l].c_str();
}

int Buffer::numlines(){
    return data.size();
}

std::string vec_to_string(std::vector<std::string>::iterator first, std::vector<std::string>::iterator last){
    std::vector<std::string>::iterator it = first;
    std::string out = "";
    while(it != last){
        out += *it + " ";
        it++;
    }
    return out;
}
    
//inserts a char into the buffer, and updates the cursor position accordingly
int Buffer::insert(char c, int& row, int& col){
    int effrow = row - 1; //Effective row: subtract 1 due to the offset of the text within the window (+1,+1)
    if(col > data[effrow].length()){ col = data[effrow].length() + 1; }
    int effcol = col - 1;
    auto it = data[effrow].begin();
    std::advance(it, effcol);
    data[effrow].insert(it, c);
    int rval = BUFF_TRIV;

    if(effcol >= columns - 1){
        rval = BUFF_CRLF;
        auto wordBegin = it;
        while(*wordBegin != ' ' && wordBegin != data[effrow].begin()){
            wordBegin--;
        }
        wordBegin++; //set to one after the last whitespace
        std::string part(wordBegin, data[effrow].end());
        data[effrow].erase(wordBegin, data[effrow].end());
        row++;
        effrow++;
        col = part.length() + 1;
        effcol = col - 1;
        if(effrow < data.size()){
            data[effrow].insert(0, part);
        }
        else{
            data.push_back(part);
        }
    }
    else{
        col++;
    }
    if(data[effrow].length() > columns){
        if(rval == BUFF_TRIV){ rval = BUFF_WRAP; } //CRLF overrides WRAP
        std::string tail = vec_to_string(data.begin() + effrow, data.end());
        data.erase(data.begin() + effrow, data.end());
        flow(data, tail, columns);
    }
    return rval;
}
