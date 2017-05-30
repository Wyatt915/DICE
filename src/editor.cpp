#include "editor.hpp"

#include <ctype.h>

#define EFFROW cury - margin + scroll

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
    if(curx - margin > curline.length()) { return; }
    auto rmpos = std::begin(curline);
    std::advance(rmpos, curx - margin);
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

void Editor::clear(){
    buffer.clear();
    buffer.push_back("");
}

void Editor::fill(std::string text){
    buffer.clear();
    flow(buffer, text, fieldwidth);
    update();
}

//-------------------------------------------------------------------

#include <ncurses.h>
#include <panel.h>

Editor::Editor(){
    int h, w;
    getmaxyx(stdscr, h, w);
    created = true;
    edwin = newwin(h/2, w/2, h/4, w/4);
    wmove(edwin, 1, 1);
    getyx(edwin, cury, curx);
    getmaxyx(edwin, fieldheight, fieldwidth);
    buffer.push_back("");
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
    flow(buffer, in, fieldwidth);
}

Editor::Editor(WINDOW* w){
    created = false;
    edwin = w;
    wmove(edwin, 1, 1);
    getyx(edwin, cury, curx);
    getmaxyx(edwin, fieldheight, fieldwidth);
    init();
    buffer.push_back("");
}

Editor::Editor(WINDOW* w, std::string in){
    created = false;
    edwin = w;
    wmove(edwin, 1, 1);
    getyx(edwin, cury, curx);
    getmaxyx(edwin, fieldheight, fieldwidth);
    init();
    flow(buffer, in, fieldwidth);
}

void Editor::init(){
    title = "";
    edpanel = new_panel(edwin);
    edited = false;
    mode = NORMAL;
    scroll = 0;
    margin = 1;
    fieldwidth -= 2*margin;
    fieldheight -= 2*margin;
    update();
    refresh();
}

Editor::~Editor(){
    del_panel(edpanel);
    if(created) delwin(edwin);
}

void Editor::show(){
    show_panel(edpanel);
    update_panels();
}

void Editor::hide(){
    hide_panel(edpanel);
}

void Editor::setTitle(std::string in){
    title = in;
}

void Editor::listen(){
	int c;
    doupdate();
    while((c = getch()) != KEY_F(1)){
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
    
    for(int i = 0; i + scroll < buffer.size() && i < fieldheight; i++){
        wmove(edwin, i + margin, margin);
        waddstr(edwin, rmnl(buffer[i + scroll]));
    }

    if(mode == INSERT){
        wattron(edwin, COLOR_PAIR(2));    
        box(edwin, 0, 0);
        if(title.length() > 0){
            wmove(edwin, 0, (fieldwidth - title.length() + 2)/2);
            waddch(edwin, '[');
            waddstr(edwin, title.c_str());
            waddch(edwin, ']');
        }
        mvwaddstr(edwin, fieldheight + 2*margin - 1, fieldwidth/4, "[INSERT]");
        wattroff(edwin, COLOR_PAIR(2));     
    }
    if(mode == NORMAL){
        //wattron(edwin, COLOR_PAIR(2));    
        box(edwin, 0, 0);
        if(title.length() > 0){
            wmove(edwin, 0, (fieldwidth - title.length() + 2)/2);
            waddch(edwin, '[');
            waddstr(edwin, title.c_str());
            waddch(edwin, ']');
        }
        mvwaddstr(edwin, fieldheight + 2*margin - 1, fieldwidth/4, "[NORMAL]");
        //wattroff(edwin, COLOR_PAIR(2));     
    }

    wmove(edwin, cury, curx);
    wrefresh(edwin);
    //refresh();
}

bool Editor::move_home(){
    curx = margin;
    wmove(edwin, cury, curx);
    return true;
}

bool Editor::move_end(){
    int nladj = buffer[EFFROW].back() == '\n' ? 1 : 0;  //Adjustment if there is a newline char.  we do not want to be able to insert after a newline
    curx = buffer[EFFROW].length() + margin - nladj;
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
    int len = buffer[EFFROW].length();
    
    if(len >= 0 && curx - margin > len){
        move_end();
    }
    
    wmove(edwin, cury, curx);
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
        
        wmove(edwin, cury, curx);
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
    if(curx - margin <= buffer[EFFROW].length() && buffer[EFFROW][curx - margin] == '\n'){
        if(move_down()){
            return move_home();
        }
        else{
            return false;
        }
    }
    curx++;
    if(curx <= buffer[EFFROW].length() + margin){
        wmove(edwin, cury, curx);
        return true;
    }
    else if(move_down()){
        return move_home();
    }
    return false;
}

void Editor::process(int c){
    getyx(edwin, cury, curx);
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
                while((curx - margin) % 4 != 0){
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
                wmove(edwin, cury, curx);
                update();
            }
    }

    wmove(edwin, cury, curx);
    wrefresh(edwin);
}
