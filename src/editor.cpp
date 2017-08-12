#include "editor.hpp"

#include <ctype.h>

#define EFFROW cury - margin[MTOP] + scroll

void flow(std::vector<std::string>& wrap,std::string in, int width){
    std::string::iterator wspos = std::begin(in); //position of last whitespace
    std::string::iterator it = std::begin(in);
    std::string::iterator first = std::begin(in);
    std::string line;
    while(it != in.end()){
        if(std::distance(first, it) >= width){
            line = std::string(first, ++wspos);
            it = wspos;
            if(line.length() <= width){
                wrap.push_back(line); //TODO: Handle cases where a word may be longer than the width
                first = wspos;
            }
            else{
                while(std::distance(first, it) >= width){
                    it--;
                }
                wrap.push_back(std::string(first, it));
                first = it;
                it++;
            }
            continue;
        }
        else if(*it == '\n'){
            line = std::string(first, ++it);
            if(line.length() < width){
                wrap.push_back(line); //TODO: Handle cases where a word may be longer than the width
                first = it;
                wspos = it;
            }
            else{
                while(std::distance(first, it) >= width){
                    it--;
                }
                wrap.push_back(std::string(first, it));
                first = it;
                it++;
            }
            continue;
        }
        else if(isspace(*it)){
            wspos = it;
        }
        it++;
    }
    wrap.push_back(std::string(first, it)); 
}

std::string collapse(std::vector<std::string>::iterator line, std::vector<std::string>::iterator last){
    std::string out;
    while(line != last){
        out += *line;
        line++;
    }
    return out;
}

//when at the end of the line, find where in the current (possibly incomplete) word we are.
int posInWord(std::string::iterator it){
    int p = 0;
    while(!isspace(*it)){
        p++;
        it--;
    }
    return p;
}

void Editor::wrap(int row){
    auto txtit = std::begin(buffer);
    std::advance(txtit, row);
    //from the current row to the end of the buffer, add each row to a single string.
    std::string tail = collapse(txtit, std::end(buffer));
    //remove all rows from the current to the end
    buffer.erase(txtit, std::end(buffer));
    //reflow the buffer in tail, and put it back in the buffer.
    flow(buffer, tail, fieldwidth);
}

void Editor::insert(char c){
    int row = EFFROW;
    std::string& curline = buffer[row];
    if(curx - margin[MLFT] > curline.length()) { return; }
    auto inspos = std::begin(curline);
    std::advance(inspos, curx - margin[MLFT]);
    curline.insert(inspos, c);
    curx++;
    if(c == '\n'){
        wrap(row);
        move_down();
        move_home();
    }
    if(curline.length() > fieldwidth){
        int jump = posInWord(inspos);
        wrap(row);        
        if(curx - margin[MLFT] >= fieldwidth){
            curx = margin[MLFT] + jump;
            move_down();
        }
    }
    edited = true;
}

int firstWordLen(std::string in){
    auto it = std::begin(in);
    int x = 0;
    do{
        x++;
        it++;
    }while(!isspace(*it));
    return x;
}

void Editor::remove(){
    int row = EFFROW;
    auto txtit = std::begin(buffer);
    std::advance(txtit, row);
    std::string& curline  = buffer[row];
    if(curx - margin[MLFT] > curline.length()) { return; }
    auto rmpos = std::begin(curline);
    std::advance(rmpos, curx - margin[MLFT]);
    if(rmpos != std::end(curline)){
        curline.erase(rmpos);
    }
    if(curline == "" && buffer.size() > 1){
        buffer.erase(txtit);
    }
    wrap(row);
    edited = true;
    update();
}

std::string Editor::toString(){
    return collapse(std::begin(buffer), std::end(buffer));
}


void Editor::fill(std::string text){
    buffer.clear();
    flow(buffer, text, fieldwidth);
    update();
}

//--------------------------------[ncurses-related functions follow]--------------------------------

#include <ncurses.h>
#include <panel.h>

Editor::Editor():DiceWin(){
    getmaxyx(win, fieldheight, fieldwidth);
    buffer.push_back("");
    init();
}

Editor::Editor(std::string in):DiceWin(){
    wmove(win, 1, 1);
    getyx(win, cury, curx);
    getmaxyx(win, fieldheight, fieldwidth);
    flow(buffer, in, fieldwidth);
    init();
}

Editor::Editor(WinPos w):DiceWin(w){
    getmaxyx(win, fieldheight, fieldwidth);
    init();
    buffer.push_back("");
}

Editor::Editor(WinPos w, std::string in):DiceWin(w){
    wmove(win, 1, 1);
    getyx(win, cury, curx);
    getmaxyx(win, fieldheight, fieldwidth);
    init();
    flow(buffer, in, fieldwidth);
}

void Editor::init(){
    title = "";
    defaultcursor = 1;
    edited = false;
    mode = NORMAL;
    scroll = 0;
    fieldwidth -= margin[MLFT] + margin[MRGT];
    fieldheight -= margin[MTOP] + margin[MBOT];
    wmove(win, margin[MLFT], margin[MTOP]);
    getyx(win, cury, curx);
    update();
    doupdate();
}

Editor::~Editor(){
}

void Editor::clear(){
    buffer.clear();
    buffer.push_back("");
    wmove(win, 0, 0);
}

void Editor::listen(){
	int c;
    doupdate();
    while(!((c = getch()) == 'q' && mode == NORMAL)){
		process(c);
	}
}

bool Editor::has_changed(){
    return edited;
}

//if the last char in the string is a newline, replace it with a space.
const char* rmnl(std::string in){
    if(in.back() == '\n') { in.back() = ' '; }
    return in.c_str();

}

void Editor::update(){
    //int clearfromline = cury - 1 > margin ? cury - 1 : margin;
    wmove(win, 0, 0);
    wclrtobot(win);
    
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    
    wattron(win, COLOR_PAIR(1));   
    for(int i = 0; i < fieldheight; i++){
        wmove(win, i + margin[MTOP], margin[MLFT]);
        waddch(win, '~');
    }
    wattroff(win, COLOR_PAIR(1));
        
    int tempx = margin[MLFT];
    int tempy = margin[MTOP];
    
    for(int i = 0; i + scroll < buffer.size() && i < fieldheight; i++){
        wmove(win, i + margin[MTOP], margin[MLFT]);
        waddstr(win, rmnl(buffer[i + scroll]));
    }

    if(mode == INSERT){
        wattron(win, COLOR_PAIR(2));    
        box(win, 0, 0);
        if(title.length() > 0){
            wmove(win, 0, (fieldwidth - title.length() + 2)/2);
            waddch(win, '[');
            waddstr(win, title.c_str());
            waddch(win, ']');
        }
        mvwaddstr(win, fieldheight + margin[MTOP] + margin[MBOT] - 1, fieldwidth/4, "[INSERT]");
        wattroff(win, COLOR_PAIR(2));     
    }
    if(mode == NORMAL){
        box(win, 0, 0);
        if(title.length() > 0){
            wmove(win, 0, (fieldwidth - title.length() + 2)/2);
            waddch(win, '[');
            waddstr(win, title.c_str());
            waddch(win, ']');
        }
        mvwaddstr(win, fieldheight + margin[MTOP] + margin[MBOT] - 1, fieldwidth/4, "[NORMAL]");
    }

    wmove(win, cury, curx);
    wnoutrefresh(win);
    doupdate();
}

bool Editor::move_home(){
    curx = margin[MLFT];
    wmove(win, cury, curx);
    return true;
}

bool Editor::move_end(){
    int nladj = buffer[EFFROW].back() == '\n' ? 1 : 0;  //Adjustment if there is a newline char.  we do not want to be able to insert after a newline
    curx = buffer[EFFROW].length() + margin[MLFT] - nladj;
    wmove(win, cury, curx);
    return true;
}

bool Editor::move_up(){
    cury--;
    bool res = true;
    if(cury < margin[MTOP]){
        res = false;
        cury = margin[MTOP];
        if(scroll > 0){
            scroll--;
            res = true;
            update();
        }
    }
    int len = buffer[EFFROW].length();
    
    if(len >= 0 && curx - margin[MLFT] > len){
        move_end();
    }
    
    wmove(win, cury, curx);
    return res;
}

bool Editor::move_down(){
    cury++;
    bool res = true;
    if(buffer.size() > fieldheight && cury > fieldheight){
        cury = fieldheight;
        if(fieldheight + scroll == buffer.size()){
            cury--;
        }
        if(fieldheight + scroll <= buffer.size()){
            scroll++;
        }
        
        wmove(win, cury, curx);
        update();
        return true;
    }
    if(EFFROW >= buffer.size()){
        res = false;
        cury--;
    }
    
    int len = buffer[EFFROW].length();
    
    if(len >= 0 && curx > len){
        move_end();
    }
    wmove(win, cury, curx);
    return res;
}

bool Editor::move_left(){
    if(curx > margin[MLFT]){
        curx--;
        wmove(win, cury, curx);
        return true;
    }
    else if(move_up()){
        move_end();
        return true;
    }
    return false;
}

bool Editor::move_right(){
    if(curx - margin[MLFT] <= buffer[EFFROW].length() && buffer[EFFROW][curx - margin[MLFT]] == '\n'){
        if(move_down()){
            return move_home();
        }
        else{
            return false;
        }
    }
    curx++;
    if(curx <= buffer[EFFROW].length() + margin[MLFT]){
        wmove(win, cury, curx);
        return true;
    }
    else if(move_down()){
        return move_home();
    }
    return false;
}

void Editor::process(int c){
    getyx(win, cury, curx);
    int insertType = -1;
    if(mode == NORMAL && c == 'i'){
        mode = INSERT;
        update();
        return;
    }
    //if ESC is pressed, go into normal mode.
    if(c == 27){
        mode = NORMAL;
        update();
        return;
    }

    switch (c) 
    {   
        case KEY_LEFT:
            move_left();
            break;
        case KEY_RIGHT:
            move_right();
            break;
        case KEY_UP:
            move_up();
            break;
        case KEY_DOWN:
            move_down();
            break;
        case KEY_HOME:
            move_home();
            break;
        case KEY_END:
            move_end();
            break;
        case '\t':
            if(mode == INSERT){
                insert(' ');
                while((curx - margin[MLFT]) % 4 != 0){
                    insert(' ');
                }
                update();
            }
            break;
        case KEY_ENTER:
        case 10:
        case 13:
            if(mode == INSERT){
                insert('\n');
                update();
            }
            break;
        case KEY_BACKSPACE:
            if(move_left() && mode == INSERT){
               remove();
               update();
            }
            break; 
        case KEY_DC:
            if(mode == INSERT){
                remove();
                update();
            }      
            break;
        default:
            if(mode == INSERT){
                insert(char(c));
                wmove(win, cury, curx);
                update();
            }
    }

    wmove(win, cury, curx);
    wrefresh(win);
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
