#pragma once

#include <string>
#include <vector>

typedef struct _win_st WINDOW;
typedef struct panel PANEL;

struct fruit{
    std::string name;
    std::string desc;
};

namespace sqlite {class database;}

class ListView{
    public:
        //void scroll(int);
        ListView(WINDOW*);
        ListView(sqlite::database*, WINDOW*, std::vector<fruit>);
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
        void init();
        void listen();
        void insert(char);
        void remove();
        void wrap(int);
        ~ListView();
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
        std::vector<fruit> listitems;
        WINDOW* lwin;
        PANEL* lpanel;
        sqlite::database* savefile;
};
