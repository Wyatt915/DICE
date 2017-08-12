/***************************************************************************************************
*                                                                                                  *
*                                        DICE source file                                          *
*   File:      main.cpp                                                                            *
*   Author:    Wyatt Sheffield                                                                     *
*                                                                                                  *
*   Contains the main() function, startup, and cleanup routines. Also defines a few global         *
*   variables like the database connection to the savefile and a structure to hold data about      *
*   the character in memory.                                                                       *
*                                                                                                  *
*                               Copyright (c) 2017 Wyatt Sheffield                                 *
*                                                                                                  *
***************************************************************************************************/

#include "characterdata.hpp"
#include "editor.hpp"
#include "list_items.hpp"
#include "list_skills.hpp"
#include "parse.hpp"
#include "roller.hpp"
#include "sqlite3.h"

//#include <filesystem>
#include <atomic>
#include <dirent.h>
#include <iostream>
#include <ncurses.h>
#include <panel.h>
#include <stdexcept>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>


sqlite3* savefile; //Global variable

gurps_cdata gchar_data; //global character data struct

void multiwindowtest(){
    try{
        int numwindows = 2;
        int SCREENH, SCREENW;
        getmaxyx(stdscr, SCREENH, SCREENW);
        WinPos dims;
        dims.x = 0;
        dims.y = 0;
        dims.w = SCREENW/3;
        dims.h = SCREENH;
        ListView** windows = new ListView*[numwindows];
        windows[0] = new ListSkills(dims);
        dims.x = dims.w +1;
        windows[1] = new ListItems(dims);
        
        windows[0]->show();
        windows[0]->give_focus();
        windows[1]->show();
        windows[1]->update();
        
        Roller r;
        r.hide();
        refresh();
        update_panels();
        doupdate();
        int c;
        int currentWindow = 0;
        while((c = getch()) != 'q'){
            if(c == ' '){
                windows[currentWindow]->revoke_focus();
                r.show();
                r.roll("3d6");
                r.update();
                r.listen();
                r.hide();
                windows[currentWindow]->give_focus();
                update_panels();
                //refresh();
            }
            else if (c == '\t'){
                windows[currentWindow]->revoke_focus();
                currentWindow = (++currentWindow) % numwindows;
                windows[currentWindow]->give_focus();
            }
            else{
                windows[currentWindow]->process(c);
            }
        }
        delete windows[0];
        delete windows[1];
        delete[] windows;
        endwin();
    }
    catch(std::exception& e){
        endwin();
        std::cout << e.what() << "\n";
        getch();
        throw e;
    }
}


void start_curses(){
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);            
    start_color();
    set_escdelay(10);
    curs_set(0);
    //if(can_change_color()){
    //    init_color(COLOR_BLACK, 137, 173, 192);
    //    init_color(COLOR_BLUE, 100, 300, 700);
    //    init_color(COLOR_YELLOW, 941, 776, 454);
    //}   
    //else{
    //    move(3,0);
    //    addstr("Custom colors not supported on this terminal.");
    //} 
}

void open(char* f){
    //namespace fs = std::filesystem;
    std::string filename = "";
    if(f){
        filename = f;
    }
    int rc = -1;
    while(rc != SQLITE_OK){
        if(filename == ""){
            DIR *dpdf;
            struct dirent *epdf;
            dpdf = opendir("./");
            std::vector<std::string> fileList;
            if (dpdf){
                std::string currentfile;
                int i = 0;
                while(epdf = readdir(dpdf)){
                    currentfile = epdf->d_name;
                    if(currentfile.find(".rpg") != std::string::npos){
                        std::cout << i << ": " << currentfile << '\n';
                        fileList.push_back(currentfile);
                        i++;
                    }
                }
                closedir(dpdf);
                std::cout << "Please select one of the numbers listed above, or enter a filename:\n";
                int choice;
                std::cin >> choice;
                filename = fileList[choice];
            }
        }
        rc = sqlite3_open_v2(filename.c_str(), &savefile, SQLITE_OPEN_READWRITE, NULL);
    }
}

void marquee(std::atomic<bool>& keepgoing){
    std::string msg = "This marquee is too long to display all at once.";
    std::string mrq;
    int start = 0;
    int l = msg.length();
    while(keepgoing){
        for(int i = 0; i < 10; i++){
            mrq += msg[(i + start) % l];
        }
        mvaddstr(30, 125, mrq.c_str());
        refresh();
        mrq = "";
        start++;
        start %= l;
        usleep(250000);
    }
}

int main(int argc, char* argv[]){
    try{
        open(argv[1]);
        start_curses();
        load_cdata();
        //lvtest();
        //std::atomic<bool> keepgoing {true};
        //std::thread marqueethread(marquee, std::ref(keepgoing));
        multiwindowtest();
        //keepgoing = false;
        //marqueethread.join();
        if(savefile) sqlite3_close(savefile);
    }
    catch(std::runtime_error& e){
        if(savefile) sqlite3_close(savefile);
        endwin();
        std::cerr << e.what() << "\n";
        return 1;
    }
    catch(std::exception& e){
        if(savefile) sqlite3_close(savefile);
        endwin();
        std::cerr << e.what() << "\n";
        return 1;
    }
    endwin();
    return 0;
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
