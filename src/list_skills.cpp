#include "list_skills.hpp"
#include "editor.hpp"
#include <sqlite_modern_cpp.h>
#include <ncurses.h>
#include <panel.h>

std::string find_rel_lvl(skill s){
    int level;
    int points = s.pnts;
    //gives the level with 1 point invested
    switch (s.diff){
        case easy:
            level = 0;
            break;
        case average:
            level = -1;
            break;
        case hard:
            level = -2;
            break;
        case veryhard:
            level = -3;
            break;
    }
    
    if(points == 2 || points == 3){
        level += 1;
    }
    else if (points >= 4){
        level += 1 + points/4;
    }
    bool negative = level < 0;
    std::string out;
    if(negative){
        out = s.base + std::to_string(level);
    }
    else{
        out = s.base+ "+" + std::to_string(level);
    }
    return out;
}

ListSkills::ListSkills(sqlite::database* db, WINDOW* win, std::vector<skill> l){
    savefile = db;
    curs_set(0); //invisible cursor
    lwin = win;
    getmaxyx(lwin, fieldheight, fieldwidth);
    wmove(lwin, 1, 1);
    getyx(lwin, cury, curx);
    listitems = l;
    init();
}

ListSkills::~ListSkills(){
    curs_set(1);
}

void ListSkills::init(){
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

void ListSkills::give_focus(){
    has_focus = true;
    update();
}

void ListSkills::revoke_focus(){
    has_focus = false;
    update();
}

void ListSkills::update(){
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
        mvwaddstr(lwin, i + margin, margin + 20, find_rel_lvl(listitems[i+scroll]).c_str());
        std::string invested = "[" + std::to_string(listitems[i + scroll].pnts) + "]";
        mvwaddstr(lwin, i + margin, margin + 28, invested.c_str());
        wmove(lwin, i + margin, margin);
    }
    if(has_focus){ wattron(lwin, COLOR_PAIR(2)); }
    box(lwin, 0, 0);
    if(has_focus){ wattroff(lwin, COLOR_PAIR(2)); }
    wmove(lwin, cury, curx);
    wrefresh(lwin);
}

void ListSkills::listen(){
    int c;
    while((c = getch()) != KEY_F(1)){
        process(c);
        update();
    }
}

bool ListSkills::move_up(){
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

bool ListSkills::move_down(){
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

void ListSkills::addItem(){
    Editor e;
    skill newskill;
    curs_set(1);
    e.setTitle("New Item");
    e.show();
    e.listen();
    newskill.name = e.toString();
    if(newskill.name.length() > 0){
        e.clear();
        e.setTitle("Enter Description");
        e.listen();
        newskill.desc = e.toString();
        listitems.push_back(newskill);
        *savefile << "INSERT INTO skills(name, description) VALUES (?,?);"
            << newskill.name << newskill.desc;
    }
    e.hide();
    curs_set(0);
}

void ListSkills::removeItem(){

}

void ListSkills::editItem(){
    Editor e;
    curs_set(1);
    e.setTitle(listitems[selection].name);
    e.show();
    e.fill(listitems[selection].desc);
    e.update();
    e.listen();
    if(e.has_changed()){
        listitems[selection].desc = e.toString();
        *savefile << "UPDATE skills SET description = ? WHERE name = ?;"
            << listitems[selection].desc << listitems[selection].name; 
    }
    e.hide();
    curs_set(0);
}

void ListSkills::investPoints(int howmany){
    listitems[selection].pnts += howmany;
    //TODO debit/credit to unspent points
    update();
}

int ListSkills::process(int c){
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
        case '=':
        case '+':
            investPoints(1);
            break;
        case '-':
            investPoints(-1);
            break;
        case 10:
        case 13:
        case KEY_ENTER:
            editItem();
            break;
    }
}
