#include "list_skills.hpp"
#include "editor.hpp"

#include <sqlite_modern_cpp.h>
#include <ncurses.h>
#include <panel.h>
#include <ctype.h>

void read_db(sqlite::database* db, std::vector<skill>& list){
    try{    
        skill temp;
        *db << "SELECT rowid,name,base,diff,pnts FROM skills"
            >> [&](unsigned long id, std::string name, std::string base, std::string diff, int points){
                temp.id = id;
                temp.name = name;
                temp.base = base;
                temp.pnts = points;
                if(toupper(diff[0]) == 'E'){ temp.diff = easy; }
                if(toupper(diff[0]) == 'A'){ temp.diff = average; }
                if(toupper(diff[0]) == 'H'){ temp.diff = hard; }
                if(toupper(diff[0]) == 'V'){ temp.diff = veryhard; }
                list.push_back(temp);
        };
    }
    catch(std::exception& e){
        std::cout << e.what();
    }
}

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

ListSkills::ListSkills(sqlite::database* db, WINDOW* win):ListView(db, win){
    curs_set(0); //invisible cursor
    
    read_db(db, listitems);
    num_items = listitems.size();

    setTitle("Skills");
    std::string head = createHeader();
    setHeader(head);
    has_footer = false;

    fieldwidth -= margin[MRGT] + margin[MLFT];
    fieldheight-= margin[MTOP] + margin[MBOT];
    update_panels();
    doupdate();
    update();
    wrefresh(lwin);
}

ListSkills::~ListSkills(){
    curs_set(1);
}

void ListSkills::show(){
    show_panel(lpanel);
}

void ListSkills::hide(){
    hide_panel(lpanel);
}

void ListSkills::setFooter(std::string f){
    footer = f;
    has_footer = true;
    margin[MBOT] = 2;
}

void ListSkills::setHeader(std::string h){
    header = h;
    has_header = true;
    margin[MTOP] = 2;
}

void ListSkills::setTitle(std::string t){
    title = t;
}

std::string ListSkills::createHeader(){
    std::string h = "";
    std::string data[4] = {"Name", "Level", "Rel. Level", "Points"};
    //TODO dynamically calculate tabstops, based on the longest word in each field
    tabstops[0] = margin[MLFT];
    tabstops[1] = margin[MLFT] + 16;
    tabstops[2] = margin[MLFT] + 24;
    tabstops[3] = margin[MLFT] + 32;
    for(int i = 0; i < 3; i++){
        h += data[i];
        while(h.length() - margin[MLFT] < tabstops[i + 1]){
            h += " ";
        }
    }
    h += data[3];
    return h;
}

void ListSkills::update(){
    wmove(lwin, 0, 0);
    wclrtobot(lwin);
    
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    
    //int tempx = margin;
    //int tempy = margin;
    
    if(has_header){
        mvwaddstr(lwin, margin[MLFT], 1, header.c_str());
    }

    for(int i = 0; i + scroll < num_items && i - scroll < fieldheight; i++){
        wmove(lwin, i + margin[MTOP], margin[MLFT]);
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
        mvwaddstr(lwin, i + margin[MTOP], tabstops[1], find_rel_lvl(listitems[i+scroll]).c_str());
        std::string invested = "[" + std::to_string(listitems[i + scroll].pnts) + "]";
        mvwaddstr(lwin, i + margin[MTOP], tabstops[3], invested.c_str());
        wmove(lwin, cury, curx);
    }
    if(has_focus){ wattron(lwin, COLOR_PAIR(2)); }
    box(lwin, 0, 0);
    if(has_focus){ wattroff(lwin, COLOR_PAIR(2)); }
    wmove(lwin, cury, curx);
    wrefresh(lwin);
}

//void ListSkills::listen(){
//    int c;
//    while((c = getch()) != KEY_F(1)){
//        process(c);
//        update();
//    }
//}

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
    //update();
}
