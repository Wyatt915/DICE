#pragma once

#include "utils.hpp"

#include <string>
#include "sqlite3.h"

#define MTOP 0  //top margin
#define MRGT 1  //right margin
#define MBOT 2  //bottom margin
#define MLFT 3  //left margin

typedef struct _win_st WINDOW;
typedef struct panel PANEL;

class ListView{
    public:
        ListView();
        ListView(sqlite3*, WinPos);
        bool move_down();
        bool move_up();
        int numlines();
        virtual int process(int) = 0;
        virtual void addItem() = 0;
        virtual void editItem() = 0;
        //virtual void init() = 0;
        void setFooter(std::string);
        void setHeader(std::string);
        void setTitle(std::string);
        virtual void update() = 0;
        void show();
        void hide();
        void give_focus();
        void listen();
        void revoke_focus();
        void wrap(int);
        virtual ~ListView();
    protected:
        bool has_footer;
        bool has_focus;
        bool has_header;
        int curx;
        int cury;
        int fieldheight;
        int fieldwidth;
        int margin[4]; //{top, right, bottom, left}
        int mode;
        int num_items;
        int scroll;
        int selection;
        std::string header;
        std::string footer;
        std::string title;
        WINDOW* lwin;
        PANEL* lpanel;
        sqlite3* savefile;
};
