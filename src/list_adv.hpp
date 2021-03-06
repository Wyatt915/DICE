//list_skills.hpp

#pragma once

#include "listview.hpp"

#include <string>
#include <vector>

struct advantage{
    unsigned long id = 0;    //database ID of the skill
    std::string name = "";   //Name of the skill
    std::string desc = "";   //description
    int points = 0;          //points invested
    int min;
    int max;
    int cost;
};

class ListAdv : public ListView{
    public:
        //void scroll(int);
        ListAdv();
        ListAdv(WinPos);
        int numlines();
        void process(int);
        void add_item();
        void remove_item();
        void edit_item();
        void investPoints(int);
        void insert(char);
        void remove();
        void read_db();
        //void show();
        //void hide();
        //virtual void update();
        //virtual void setFooter(std::string);
        //virtual void setHeader(std::string);
        //virtual void setTitle(std::string);
        std::string createHeader();
        ~ListAdv();
    private:
        bool created;
        std::vector<advantage> advantages;
};

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
