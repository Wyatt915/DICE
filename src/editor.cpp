#include "editor.hpp"
#include "buffer.hpp"

Editor::Editor(WINDOW* w){
    mode = NORMAL;
    scroll = 0;
    edwin = w;
    getyx(edwin, cury, curx);
    getmaxyx(edwin, lines, columns);
    columns -= 2;
    data = new Buffer(columns);
}

Editor::Editor(WINDOW* w, std::string in){
    mode = NORMAL;
    scroll = 0;
    edwin = w;
    getyx(edwin, cury, curx);
    getmaxyx(edwin, lines, columns);
    columns -= 2;
    data = new Buffer(in, columns);
}

Editor::~Editor(){
    delete data;
}

const char* Editor::getline(int l){
    return data->getline(l);
}

int Editor::numlines(){
    return data->numlines();
}

void Editor::update(int begin){
    if(cury + begin < 1) { begin = 0; }
    wmove(edwin, cury+begin, curx);
    wclrtobot(edwin);
    for(int i = 0; i < data->numlines() + scroll + begin; i++){
        wmove(edwin, i+1+begin, 1);
        waddstr(edwin, data->getline(i + scroll + begin));
    }
    box(edwin, 0, 0);
    wmove(edwin, cury, curx);
    wrefresh(edwin);
}

int min(int x, int y){
    return x < y ? x : y;
}

bool Editor::move_up(){
    cury--;
    bool res = true;
    if(cury < 1){
        res = false;
        cury = 1;
        if(scroll > 0){
            scroll--;
            res = true;
            update(0);
        }
    }
    wmove(edwin, cury, curx);
    return res;
}

bool Editor::move_down(){
    cury++;
    bool res = true;
    if(cury >= data->numlines()){
        res = false;
        cury = data->numlines();
        if(data->numlines() > scroll + cury){
            scroll++;
            res = true;
            update(0);
        }
    }
    wmove(edwin, cury, curx);
    return res;
}

bool Editor::move_left(){
    if(curx > 1){
        curx--;
        wmove(edwin, cury, curx);
        return true;
    }
    else if(move_up()){
        curx = columns + 1;
        wmove(edwin, cury, curx);
        return true;
    }
}

bool Editor::move_right(){
    curx++;
    if(curx <= data->getlinelen(cury)){
        wmove(edwin, cury, curx);
        return true;
    }
    if(move_down()){
        curx = 1;
        wmove(edwin, cury, curx);
        return true;
    }
    curx--;
    return false;
}

int Editor::process(int c){
    getyx(edwin, cury, curx);
    int insertType = -1;
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
        case KEY_BACKSPACE:
            if(move_left()){
               data->remove(cury, curx);
               update(0);
            }
            break; 
            case KEY_DC:
            data->remove(cury, curx);
            update(0);
            break;
        default:
            insertType = data->insert(c, cury, curx);
            wmove(edwin, cury, curx);
    }
    switch (insertType){
        case BUFF_TRIV:
        case BUFF_WRAP:
            update(0);
            break;
        case BUFF_CRLF:
            update(-1);
    }

   // box(edwin, 0, 0);
    if(curx > data->getlinelen(cury)){
        curx = data->getlinelen(cury) + 1;
    }
    wrefresh(edwin);

    return ED_INS; 
}

