#pragma once

#include <string>
#include <vector>

typedef struct _win_st WINDOW;
typedef struct panel PANEL;

enum gurps_diff { easy, average, hard, veryhard };

struct skill{
    std::string name;   //Name of the skill
    std::string base;   //base attribute or base skill
    gurps_diff  diff;   //difficulty
    std::string desc;   //description
    int pnts;           //points invested
};

std::string find_rel_lvl(skill s);

namespace sqlite {class database;}

class ListSkills{
    public:
        //void scroll(int);
        ListSkills(WINDOW*);
        ListSkills(sqlite::database*, WINDOW*, std::vector<skill>);
        int numlines();
        int process(int);
        bool move_up();
        bool move_down();
        void give_focus();
        void revoke_focus();
        void update();
        void addItem();
        void removeItem();
        void editItem();
        void investPoints(int);
        void init();
        void listen();
        void insert(char);
        void remove();
        void wrap(int);
        ~ListSkills();
    private:
        bool created;
        bool has_focus;
        int margin;
        int mode;
        int fieldheight;
        int fieldwidth;
        int scroll;
        int curx;
        int cury;
        int selection;
        std::vector<skill> listitems;
        WINDOW* lwin;
        PANEL* lpanel;
        sqlite::database* savefile;
};
