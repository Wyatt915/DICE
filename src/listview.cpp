#include "listview.hpp"
#include "editor.hpp"
#include <sqlite_modern_cpp.h>
#include <ncurses.h>

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
    margin = 1;
    fieldwidth -= 2*margin;
    fieldheight -= 2*margin;
    update();
    refresh();
}

void ListView::update(){
    wmove(lwin, 0, 0);
    wclrtobot(lwin);
    
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    /**
    wattron(lwin, COLOR_PAIR(1));   
    for(int i = 0; i < fieldheight; i++){
        wmove(lwin, i + margin, margin);
        waddch(lwin, '~');
    }
    wattroff(lwin, COLOR_PAIR(1));
    */    
    int tempx = margin;
    int tempy = margin;
    
    for(int i = scroll; i + scroll < listitems.size() && i < fieldheight; i++){
        wmove(lwin, i + margin, margin);
        if(i + scroll == selection){
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
    box(lwin, 0, 0);
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

int ListView::process(int c){
    Editor e;
    e.hide();
    fruit newfruit;
    switch(c){
        case KEY_UP:
            selection = selection > 0 ? selection - 1 :  listitems.size() - 1;
            break;
        case KEY_DOWN:
            selection = (selection + 1) % listitems.size();
            break;
        case 'a':
            curs_set(1);
            e.setTitle("New Item");
            e.show();
            e.listen();
            newfruit.name = e.toString();
            e.clear();
            e.setTitle("Enter Description");
            e.listen();
            newfruit.desc = e.toString();
            listitems.push_back(newfruit);
            *savefile << "INSERT INTO fruits(name, description) VALUES (?,?);"
                << newfruit.name << newfruit.desc;
            e.hide();
            curs_set(0);
            break;
        case 10:
        case 13:
        case KEY_ENTER:
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
            clear();
            refresh();
            break;
    }
}
