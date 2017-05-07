#include "buffer.hpp"
#include <algorithm>

void flow(std::vector<std::string>& wrap,std::string in, int columns){
    std::string::iterator wspos = in.begin(); //position of last whitespace
    std::string::iterator it = in.begin();
    std::string::iterator first = in.begin();
    
    while(it != in.end()){
        if(std::distance(first, it) >= columns){
            wrap.push_back(std::string(first, wspos)); //TODO: Handle cases where a word may be longer than the number of columns
            first = wspos;
            first++;
            it = first;
            continue;
        }
        else if(*it == ' '){ wspos = it; }
        it++;
    }
    wrap.push_back(std::string(first, it)); 
}

Buffer::Buffer(){
    columns = 80;
    raw = "";
    disp.push_back(raw);
    //lines = 1;
    margin = 1;
}

Buffer::Buffer(int c) : columns(c){
    //lines = 1;
    raw = "";
    disp.push_back(raw);
    margin = 1;
}

Buffer::Buffer(std::string in){
    raw = in;
    columns = 80;
    flow(disp, raw, columns);
    //lines = disp.size();
    margin = 1;
}

Buffer::Buffer(std::string in, int c) : columns(c) {
    raw = in;
    flow(disp, raw, columns);
    //lines = disp.size();
    margin = 1;
}

int effcol(){
    return 0;
}

std::string Buffer::to_string(){
    std::string out = "";
    for(std::string l : disp){
        out += l + "\n\r";
    }
    return out;
}

const char* Buffer::getline(int l){
    return disp[l].c_str();
}

int Buffer::getlinelen(int row){
    return disp[row - margin].length();
}

int Buffer::numlines(){
    return disp.size();
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

/*
//inserts a char into the buffer, and updates the cursor position accordingly
int Buffer::insert(char c, int& row, int& col){
    int effrow = row - margin; //Effective row: subtract 1 due to the offset of the text within the window (+1,+1)
    if(col > disp[effrow].length()){ col = disp[effrow].length() + 1; }
    int effcol = col - margin;
    auto it = disp[effrow].begin();
    std::advance(it, effcol);
    disp[effrow].insert(it, c);
    int rval = BUFF_TRIV;

    if(effcol >= columns - 1){
        rval = BUFF_CRLF;
        auto wordBegin = it;
        while(*wordBegin != ' ' && wordBegin != disp[effrow].begin()){
            wordBegin--;
        }
        wordBegin++; //set to one after the last whitespace
        std::string part(wordBegin, disp[effrow].end());
        disp[effrow].erase(wordBegin, disp[effrow].end());
        row++;
        effrow++;
        col = part.length() + 1;
        effcol = col - margin;
        if(effrow < disp.size()){
            disp[effrow].insert(0, part);
        }
        else{
            disp.push_back(part);
        }
    }
    else{
        col++;
    }
    if(disp[effrow].length() > columns){
        if(rval == BUFF_TRIV){ rval = BUFF_WRAP; } //CRLF overrides WRAP
        std::string tail = vec_to_string(disp.begin() + effrow, disp.end());
        disp.erase(disp.begin() + effrow, disp.end());
        flow(disp, tail, columns);
    }
    return rval;
}



int Buffer::remove(int& row, int& col){
    int effrow = row - margin; //Effective row: subtract 1 due to the offset of the text within the window (+1,+1)
    if(col > disp[effrow].length()){ col = disp[effrow].length() + 1; }
    int effcol = col - margin;
    auto it = disp[effrow].begin();
    std::advance(it, effcol);
    disp[effrow].erase(it);
    if(effrow < disp.size() - 1){
        auto wordBegin = disp[effrow+1].begin();
        auto wordEnd = wordBegin;
        while(*wordEnd != ' ' && wordEnd != disp[effrow].end()){
            wordEnd++;
        }
        if(std::distance(wordBegin, wordEnd) <= columns - disp[effrow].size()){
            std::string tail = vec_to_string(disp.begin() + effrow, disp.end());
            disp.erase(disp.begin() + effrow, disp.end());
            flow(disp, tail, columns);
        }
    }
    return 0;
}
*/

const char* Buffer::debug(){
    return debuginfo.c_str();
}

int Buffer::find_pos_in_raw(int row, int col){
    int pos = 0;
    row -= margin;
    col -= margin;
    int i = 0; int j = 0;
    bool brflag = false;
    while(!brflag){
        if(i < row){
            pos += disp[i].length();
        }
        else{
            while(j < col && j < disp[row].length()){
                pos++;
                j++;
            }
            brflag = true;
        }
        i++;
    }
    std::stringstream ss;
    ss << "coord( " << row << ", " << col << ") -->  position: " << pos << "\n";
    debuginfo = ss.str();
    return pos - 1;
}

void Buffer::get_coord_from_pos(int& row, int& col, int pos){
    int accum = 0;
    row = 0;
    col = 0;
    bool brflag = false;
    for(int i = 0; i < disp.size(); i++){
        for(int j = 0; j < disp[i].length(); j++){
            if(accum == pos){
                row = i;
                col = j;
            }
            accum++;
        }
    }
    //row--; //the loop increments row one too many times.
    //int overshoot = accum - pos;
    //col = disp[row].length() - overshoot + margin;
    std::stringstream ss;
    ss << "position(" << pos << ") --> Coordinate: (" << row << ", " << col << ')';
    debuginfo += ss.str();
    row += margin;
    col += margin;
}

int Buffer::insert(char c, int& row, int& col){
    int pos = find_pos_in_raw(row, col);
    auto it = std::begin(raw);
    std::advance(it, pos);
    raw.insert(it, c);
    pos++;
    disp.clear();
    flow(disp, raw, columns);
    find_pos_in_raw(row, col);
    get_coord_from_pos(row, col, pos);
    return 0;
}

int Buffer::remove(int& row, int& col){
    if(raw.length() == 0){
        return 0;
    }
    
    int pos = find_pos_in_raw(row, col);
    auto it = std::begin(raw);
    int lc = 0; //loop count
    while(it != std::end(raw) && lc < pos){
        it++;
        lc++;
    }
    if(it == std::end(raw)){
        return 0;
    }
    //it--;
    raw.erase(it);
    disp.clear();
    flow(disp, raw, columns);
    return 0;
}
