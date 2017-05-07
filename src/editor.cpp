#include "editor.hpp"
#include "buffer.hpp"

Editor::Editor(WINDOW* w){
    mode = NORMAL;
    scroll = 0;
    edwin = w;
    getyx(edwin, cury, curx);
    getmaxyx(edwin, lines, columns);
    margin = 1;
    columns -= 2 * margin;
    lines -= 2*margin;
    data = new Buffer(columns);
    update();
}

Editor::Editor(WINDOW* w, std::string in){
    mode = NORMAL;
    scroll = 0;
    edwin = w;
    getyx(edwin, cury, curx);
    getmaxyx(edwin, lines, columns);
    margin = 1;
    columns -= 2 * margin;
    lines -= 2*margin;
    data = new Buffer(in, columns);
    update();
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

void Editor::update(){
    wmove(edwin, 0, 0);
    wclrtobot(edwin);
    int i;
    for(i = 0; i < data->numlines() && i < lines; i++){
        wmove(edwin, i + margin, margin);
        waddstr(edwin, data->getline(i + scroll));
    }

    
    //start_color();

    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    wattron(edwin, COLOR_PAIR(1));
    
    for(; i < lines; i++){
        wmove(edwin, i + margin, margin);
        waddch(edwin, '~');
    }
    
    //wrefresh(edwin);
    wattroff(edwin, COLOR_PAIR(1));

    box(edwin, 0, 0);
    wmove(edwin, cury, curx);
    wrefresh(edwin);
    //refresh();
}

int min(int x, int y){
    return x < y ? x : y;
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
            update();
        }
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
        curx = data->getlinelen(columns) + margin;
        wmove(edwin, cury, curx);
        return true;
    }
}

bool Editor::move_right(){
    curx++;
    if(curx <= data->getlinelen(cury) + margin){
        wmove(edwin, cury, curx);
        return true;
    }

    if(move_down()){
        curx = margin;
        wmove(edwin, cury, curx);
        return true;
    }
    curx--;
    wmove(edwin, cury, curx);
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
               update();
            }
            break; 
        case KEY_DC:
            data->remove(cury, curx);
            update();
            break;
        default:
            insertType = data->insert(c, cury, curx);
            wmove(edwin, cury, curx);
            update();
    }

    //box(edwin, 0, 0);
    //if(curx > data->getlinelen(cury)){
    //    curx = data->getlinelen(cury) + 1;
    //}

    move(0,0);
    addstr(data->debug());
    wmove(edwin, cury, curx);
    refresh();
    wrefresh(edwin);

    return ED_INS; 
}

