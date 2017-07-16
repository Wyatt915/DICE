#include "list_skills.hpp"
#include "editor.hpp"

#include <ncurses.h>
#include <panel.h>
#include <ctype.h>
#include "sqlite3.h"
#include <sstream>

gurps_diff str_to_enum(std::string in){
    if(toupper(in[0]) == 'E'){ return easy; }
    if(toupper(in[0]) == 'A'){ return average; }
    if(toupper(in[0]) == 'H'){ return hard; }
    if(toupper(in[0]) == 'V'){ return veryhard; }
}

static int populate(void* data, int argc, char** argv, char** azColName){
    skill temp;
    std::string diff;
    std::stringstream(argv[0]) >> temp.id;
    std::stringstream(argv[1]) >> temp.name;
    std::stringstream(argv[2]) >> temp.base;
    std::stringstream(argv[3]) >> diff;
    std::stringstream(argv[4]) >> temp.pnts;
    temp.diff = str_to_enum(diff);
    std::vector<skill>* skills  = reinterpret_cast<std::vector<skill>*>(data);
    skills->push_back(temp);
    return 0;
}
    

void read_db(sqlite3* db, std::vector<skill>& list){
    skill temp;
    const char* sql = "SELECT rowid,name,base,diff,pnts FROM skills";
    char* zErrMsg = 0;
    int rc = sqlite3_exec(db, sql, populate, (void*)&list, &zErrMsg);
    if(rc != SQLITE_OK){
        std::string errstr = "QLite error: ";
        errstr += zErrMsg;
        sqlite3_free(zErrMsg);
        throw std::runtime_error(errstr);
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

//-------------------------------------[Constructor/Destructor]-------------------------------------

ListSkills::ListSkills(sqlite3* db, WinPos def):ListView(db, def){
    curs_set(0); //invisible cursor
    try{
        read_db(db, listitems);
    }
    catch(std::exception& e){
        mvaddstr(0, 0, e.what());
        throw e;
    }
    num_items = listitems.size();

    setTitle("Skills");
    std::string head = createHeader();
    setHeader(head);
    has_footer = false;

    fieldwidth -= margin[MRGT] + margin[MLFT];
    fieldheight-= margin[MTOP] + margin[MBOT];
    curx = margin[MLFT];
    cury = margin[MTOP];
    update_panels();
    doupdate();
    update();
    wrefresh(lwin);
}

ListSkills::~ListSkills(){
    mvaddstr(6, 0, "Derived Destructor");
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

//---------------------------------------[ncurses functions]---------------------------------------

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
        mvwaddstr(lwin, i + margin[MTOP], margin[MLFT] + tabstops[1] + 1, find_rel_lvl(listitems[i+scroll]).c_str());
        std::string invested = "[" + std::to_string(listitems[i + scroll].pnts) + "]";
        mvwaddstr(lwin, i + margin[MTOP], margin[MLFT] + tabstops[3] + 1, invested.c_str());
        wmove(lwin, cury, curx);
    }
    if(has_focus){ wattron(lwin, COLOR_PAIR(2)); }
    box(lwin, 0, 0);
    if(has_focus){ wattroff(lwin, COLOR_PAIR(2)); }
    wmove(lwin, cury, curx);
    wrefresh(lwin);
}

//----------------------------------------[Read/Write/Edit]----------------------------------------

void ListSkills::addItem(){
    Editor e;
    skill newskill;
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
        //savefile << "INSERT INTO skills(name, description) VALUES (?,?);"
        //    << newskill.name << newskill.desc;
    }
    e.hide();
}

void ListSkills::removeItem(){

}

void ListSkills::editItem(){
    Editor e;
    e.setTitle(listitems[selection].name);
    e.show();
    e.fill(listitems[selection].desc);
    e.update();
    e.listen();
    if(e.has_changed()){
        listitems[selection].desc = e.toString();
        //savefile << "UPDATE skills SET description = ? WHERE name = ?;"
        //    << listitems[selection].desc << listitems[selection].name; 
    }
    e.hide();
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
    update();
}

//////////////////////////////////////////////////////
//                                                  //
//      oooooooooo.    o8o                          //
//      `888'   `Y8b   `"'                          //
//       888      888 oooo   .ooooo.   .ooooo.      //
//       888      888 `888  d88' `"Y8 d88' `88b     //
//       888      888  888  888       888ooo888     //
//       888     d88'  888  888   .o8 888    .o     //
//      o888bood8P'   o888o `Y8bod8P' `Y8bod8P'     //
//                                                  //
//////////////////////////////////////////////////////
