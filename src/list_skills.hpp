#pragma once

#include "listview.hpp"

#include <string>
#include <vector>

enum gurps_diff { easy, average, hard, veryhard };

struct skill{
    unsigned long id;   //database ID of the skill
    std::string name;   //Name of the skill
    std::string base;   //base attribute or base skill
    gurps_diff  diff;   //difficulty
    std::string desc;   //description
    int pnts;           //points invested
};

std::string find_rel_lvl(skill s);

class ListSkills : public ListView{
    public:
        //void scroll(int);
        ListSkills(WINDOW*);
        ListSkills(sqlite3*, WinPos);
        int numlines();
        int process(int);
        void addItem();
        void removeItem();
        void editItem();
        void investPoints(int);
        void insert(char);
        void remove();
        void wrap(int);
        //void show();
        //void hide();
        virtual void update();
        //virtual void setFooter(std::string);
        //virtual void setHeader(std::string);
        //virtual void setTitle(std::string);
        std::string createHeader();
        ~ListSkills();
    private:
        bool created;
        int tabstops[4];
        std::vector<skill> listitems;
};
