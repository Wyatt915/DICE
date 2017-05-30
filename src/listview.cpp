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
    //if(scroll < 0) scroll = 0;
    move(0, 20);
    std::string debug = "Scroll: " + std::to_string(scroll);
    addstr(debug.c_str());
    move(1, 20);
    debug = "Selection: " + std::to_string(selection);
    addstr(debug.c_str());
    wmove(lwin, 0, 0);
    wclrtobot(lwin);
    
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    
    int tempx = margin;
    int tempy = margin;
    
    for(int i = 0; i + scroll < listitems.size() && i - scroll < fieldheight; i++){
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

int ListView::process(int c){
    Editor e;
    e.hide();
    fruit newfruit;
    switch(c){
        case KEY_UP:
            move_up();
            break;
        case KEY_DOWN:
            move_down();
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
