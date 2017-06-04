#include "listview.hpp"
#include "editor.hpp"
#include <sqlite_modern_cpp.h>
#include <ncurses.h>
#include <panel.h>

ListView::ListView(sqlite::database* db, WINDOW* win, std::vector<fruit> l){
    savefile = db;
    curs_set(0); //invisible cursor
    lwin = win;
    getmaxyx(lwin, fieldheight, fieldwidth);
    wmove(lwin, 1, 1);
    getyx(lwin, cury, curx);
    listitems = l;
    init();
}

ListView::~ListView(){
    curs_set(1);
}

void ListView::init(){
    scroll = 0;
    selection = 0;
    has_focus = false;
    margin = 1;
    fieldwidth -= 2*margin;
    fieldheight -= 2*margin;
    lpanel = new_panel(lwin);
    doupdate();
    update();
}

void ListView::give_focus(){
    has_focus = true;
    update();
}

void ListView::revoke_focus(){
    has_focus = false;
    update();
}

void ListView::update(){
    //if(scroll < 0) scroll = 0;
    wmove(lwin, 0, 0);
    wclrtobot(lwin);
    
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    
    int tempx = margin;
    int tempy = margin;
    
    for(int i = 0; i + scroll < listitems.size() && i - scroll < fieldheight; i++){
        wmove(lwin, i + margin, margin);
        if(i + scroll == selection && has_focus){
            wattron(lwin, A_UNDERLINE);
            wattron(lwin, A_STANDOUT);
            waddstr(lwin, listitems[i + scroll].name.c_str());
            wattroff(lwin, A_UNDERLINE);
            wattroff(lwin, A_STANDOUT);
        }
        else{
            waddstr(lwin, listitems[i + scroll].name.c_str());
        }
    }
    if(has_focus){ wattron(lwin, COLOR_PAIR(2)); }
    box(lwin, 0, 0);
    if(has_focus){ wattroff(lwin, COLOR_PAIR(2)); }
    wmove(lwin, cury, curx);
    wrefresh(lwin);
}

void ListView::listen(){
    int c;
    while((c = getch()) != KEY_F(1)){
        process(c);
        update();
    }
}

bool ListView::move_up(){
    if(selection == 0){
        selection = listitems.size() - 1;
        scroll = listitems.size() - fieldheight;
        if(scroll < 0) scroll = 0;
        return true;
    }
    else if(selection == scroll){
        selection--;
        scroll--;
        return true;
    }
    else{
        selection--;
        return true;
    }
}

bool ListView::move_down(){
    if(selection == listitems.size() - 1){
        scroll = 0;
        selection = 0;
        return true;
    }
    else if(selection == scroll + fieldheight - 1){
        scroll++;
        selection++;
        return true;
    }
    else{
        selection++;
        return true;
    }
}

void ListView::addItem(){
    Editor e;
    fruit newfruit;
    curs_set(1);
    e.setTitle("New Item");
    e.show();
    e.listen();
    newfruit.name = e.toString();
    if(newfruit.name.length() > 0){
        e.clear();
        e.setTitle("Enter Description");
        e.listen();
        newfruit.desc = e.toString();
        listitems.push_back(newfruit);
        *savefile << "INSERT INTO fruits(name, description) VALUES (?,?);"
            << newfruit.name << newfruit.desc;
    }
    e.hide();
    curs_set(0);
}

void ListView::removeItem(){

}

void ListView::editItem(){
    Editor e;
    curs_set(1);
    e.setTitle(listitems[selection].name);
    e.show();
    e.fill(listitems[selection].desc);
    e.update();
    e.listen();
    if(e.has_changed()){
        listitems[selection].desc = e.toString();
        *savefile << "UPDATE fruits SET description = ? WHERE name = ?;"
            << listitems[selection].desc << listitems[selection].name; 
    }
    e.hide();
    curs_set(0);
}

int ListView::process(int c){
    switch(c){
        case KEY_UP:
            move_up();
            break;
        case KEY_DOWN:
            move_down();
            break;
        case 'a':
            addItem();
            break;
        case 'x':
        case KEY_DC:
            removeItem();
            break;
        case 10:
        case 13:
        case KEY_ENTER:
            editItem();
            break;
    }
}
