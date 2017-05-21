#include "editor.hpp"

#include <ctype.h>

#define EFFROW cury - margin + scroll

void flow(std::vector<std::string>& wrap,std::string in, int width){
    std::string::iterator wspos = std::begin(in); //position of last whitespace
    std::string::iterator it = std::begin(in);
    std::string::iterator first = std::begin(in);
    
    while(it != in.end()){
        if(std::distance(first, it) >= width){
            wrap.push_back(std::string(first, ++wspos)); //TODO: Handle cases where a word may be longer than the width
            first = wspos;
            //first++;
            it = first;
            continue;
        }
        else if(*it == '\n'){
            wrap.push_back(std::string(first, ++it)); //TODO: Handle cases where a word may be longer than the width
            first = it;
            wspos = it;
            //it++;
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
    auto txtit = std::begin(text);
    std::advance(txtit, row);
    std::string tail = collapse(txtit, std::end(text));
    text.erase(txtit, std::end(text));
    flow(text, tail, fieldwidth);
}

void Editor::insert(char c){
    int row = EFFROW;
    std::string& curline = text[row];
    if(curx - margin > curline.length()) { return; }
    auto inspos = std::begin(curline);
    std::advance(inspos, curx - margin);
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
        if(curx - margin >= fieldwidth){
            curx = margin + jump;
            move_down();
        }
    }
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
    auto txtit = std::begin(text);
    std::advance(txtit, row);
    std::string& curline  = text[row];
    if(curx - margin > curline.length()) { return; }
    auto rmpos = std::begin(curline);
    std::advance(rmpos, curx - margin);
    if(rmpos != std::end(curline)){
        curline.erase(rmpos);
    }
    if(curline == "" && text.size() > 1){
        text.erase(txtit);
    }
    wrap(row);
    update();
}

std::string Editor::toString(){
    return collapse(std::begin(text), std::end(text));
}

//-------------------------------------------------------------------

#include <ncurses.h>

Editor::Editor(){
    int h, w;
    getmaxyx(stdscr, h, w);
    created = true;
    edwin = newwin(h/2, w/2, h/4, w/4);
    wmove(edwin, 1, 1);
    getyx(edwin, cury, curx);
    getmaxyx(edwin, fieldheight, fieldwidth);
    text.push_back("");
    init();
}

Editor::Editor(std::string in){
    int h, w;
    getmaxyx(stdscr, h, w);
    created = true;
    edwin = newwin(h/2, w/2, h/4, w/4);
    wmove(edwin, 1, 1);
    getyx(edwin, cury, curx);
    getmaxyx(edwin, fieldheight, fieldwidth);
    init();
    flow(text, in, fieldwidth);
}

Editor::Editor(WINDOW* w){
    created = false;
    edwin = w;
    wmove(edwin, 1, 1);
    getyx(edwin, cury, curx);
    getmaxyx(edwin, fieldheight, fieldwidth);
    init();
    text.push_back("");
}

Editor::Editor(WINDOW* w, std::string in){
    created = false;
    edwin = w;
    wmove(edwin, 1, 1);
    getyx(edwin, cury, curx);
    getmaxyx(edwin, fieldheight, fieldwidth);
    init();
    flow(text, in, fieldwidth);
}

void Editor::init(){
    mode = NORMAL;
    scroll = 0;
    margin = 1;
    fieldwidth -= 2*margin;
    fieldheight -= 2*margin;
    update();
    refresh();
}

Editor::~Editor(){
    //if(created) delwin(edwin);
}

void Editor::listen(){
	int c;
    while((c = getch()) != KEY_F(1)){
		process(c);
	}
	//delwin(edwin);
}

//if the last char in the string is a newline, replace it with a space.
const char* rmnl(std::string in){
    if(in.back() == '\n') { in.back() = ' '; }
    return in.c_str();

}

void Editor::update(){
    //int clearfromline = cury - 1 > margin ? cury - 1 : margin;
    wmove(edwin, 0, 0);
    wclrtobot(edwin);
    
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    
    wattron(edwin, COLOR_PAIR(1));   
    for(int i = 0; i < fieldheight; i++){
        wmove(edwin, i + margin, margin);
        waddch(edwin, '~');
    }
    wattroff(edwin, COLOR_PAIR(1));
        
    int tempx = margin;
    int tempy = margin;
    
    for(int i = 0; i + scroll < text.size() && i < fieldheight; i++){
        wmove(edwin, i + margin, margin);
        waddstr(edwin, rmnl(text[i + scroll]));
    }

    if(mode == INSERT){
        wattron(edwin, COLOR_PAIR(2));    
        box(edwin, 0, 0);
        mvwaddstr(edwin, fieldheight + 2*margin - 1, fieldwidth/4, "[INSERT]");
        wattroff(edwin, COLOR_PAIR(2));     
    }
    if(mode == NORMAL){
        //wattron(edwin, COLOR_PAIR(2));    
        box(edwin, 0, 0);
        mvwaddstr(edwin, fieldheight + 2*margin - 1, fieldwidth/4, "[NORMAL]");
        //wattroff(edwin, COLOR_PAIR(2));     
    }

    wmove(edwin, cury, curx);
    wrefresh(edwin);
    //refresh();F
}

bool Editor::move_home(){
    curx = margin;
    wmove(edwin, cury, curx);
    return true;
}

bool Editor::move_end(){
    int nladj = text[EFFROW].back() == '\n' ? 1 : 0;  //Adjustment if there is a newline char.  we do not want to be able to insert after a newline
    curx = text[EFFROW].length() + margin - nladj;
    wmove(edwin, cury, curx);
    return true;
}

bool Editor::move_up(){
    cury--;
    bool res = true;
    if(cury < margin){
        res = false;
        cury = margin;
        if(scroll > 0){
            scroll--;
            res = true;
            update();
        }
    }
    int len = text[EFFROW].length();
    
    if(len >= 0 && curx - margin > len){
        move_end();
    }
    
    wmove(edwin, cury, curx);
    return res;
}

bool Editor::move_down(){
    cury++;
    bool res = true;
    if(text.size() > fieldheight && cury > fieldheight){
        cury = fieldheight;
        if(fieldheight + scroll == text.size()){
            cury--;
        }
        if(fieldheight + scroll <= text.size()){
            scroll++;
        }
        
        wmove(edwin, cury, curx);
        update();
        return true;
    }
    if(EFFROW >= text.size()){
        res = false;
        cury--;
    }
    
    int len = text[EFFROW].length();
    
    if(len >= 0 && curx > len){
        move_end();
    }
    wmove(edwin, cury, curx);
    return res;
}

bool Editor::move_left(){
    if(curx > margin){
        curx--;
        wmove(edwin, cury, curx);
        return true;
    }
    else if(move_up()){
        move_end();
        return true;
    }
    return false;
}

bool Editor::move_right(){
    curx++;
    if(curx <= text[EFFROW].length() + margin){
        if(text[EFFROW][curx - margin] == '\n'){
            if(move_down()){
                return move_home();
            }
        }
        
        wmove(edwin, cury, curx);
        return true;
    }
    else if(move_down()){
        move_home();
        return true;
    }
    return false;
}

int Editor::process(int c){
    getyx(edwin, cury, curx);
    int insertType = -1;
    if(mode == NORMAL && c == 'i'){
        mode = INSERT;
        update();
        return 0;
    }
    //if ESC is pressed, go into normal mode.
    if(c == 27){
        mode = NORMAL;
        update();
        return 0;
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
        case KEY_ENTER:
        case 10:
        case 13:
            if(mode == INSERT){
                insert('\n');
                update();
            }
            break;
        case KEY_BACKSPACE:
            if(move_left()){
               remove();
               update();
            }
            break; 
        case KEY_DC:
            remove();
            update();
            break;
        default:
            if(mode == INSERT){
                insert(char(c));
                wmove(edwin, cury, curx);
                update();
            }
    }

    //box(edwin, 0, 0);
    //if(curx > data->getlinelen(cury)){
    //    curx = data->getlinelen(cury) + 1;
    //}

    wmove(edwin, cury, curx);
    refresh();
    wrefresh(edwin);

    return ED_INS; 
}

