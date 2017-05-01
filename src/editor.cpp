#include "editor.hpp"
#include "buffer.hpp"

Editor::Editor(WINDOW* w){
    mode = NORMAL;
    edwin = w;
    getmaxyx(edwin, lines, columns);
    columns -= 2;
    data = new Buffer(columns);
}

Editor::Editor(WINDOW* w, std::string in){
    mode = NORMAL;
    edwin = w;
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

int Editor::process(int c){
    int cx, cy, mx, my;
    getyx(edwin, cy, cx);
    getmaxyx(edwin, my, mx);
    int scrollOffset = 0;
    int insertType = -1;
    switch (c) 
    {   
        case KEY_LEFT:
            if(cx > 1) cx--;
            break;
        case KEY_RIGHT:
            if(cx < mx - 1)
            cx++;
            break;
        case KEY_UP:
            cy--;
            break;
        case KEY_DOWN:
            cy++;
            break;
        default:
            insertType = data->insert(c, cy, cx);
    }
    switch (insertType){
        case BUFF_TRIV:
            wclrtoeol(edwin);
            break;
        case BUFF_WRAP:
            wclrtobot(edwin);
            break;
        case BUFF_CRLF:
            wmove(edwin, cy-1, cx);
            wclrtobot(edwin);
    }

    box(edwin, 0, 0);

    if(insertType > -1){
        for(int i = 0; i < data->numlines(); i++){
            wmove(edwin, i+1, 1);
            waddstr(edwin, data->getline(i));
        }
    }

    wmove(edwin, cy, cx);
    wrefresh(edwin);

    return ED_INS; 
}

