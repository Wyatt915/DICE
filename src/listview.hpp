#pragma once

#include <string>

#define MTOP 0  //top margin
#define MRGT 1  //right margin
#define MBOT 2  //bottom margin
#define MLFT 3  //left margin

typedef struct _win_st WINDOW;
typedef struct panel PANEL;
namespace sqlite {class database;}

class ListView{
    public:
        ListView();
        ListView(sqlite::database*, WINDOW*);
        bool move_down();
        bool move_up();
        int numlines();
        virtual int process(int) = 0;
        virtual void addItem() = 0;
        virtual void editItem() = 0;
        //virtual void init() = 0;
        virtual void setFooter(std::string) = 0;
        virtual void setHeader(std::string) = 0;
        virtual void setTitle(std::string) = 0;
        virtual void update() = 0;
        void give_focus();
        void listen();
        void revoke_focus();
        void wrap(int);
        ~ListView();
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
        sqlite::database* savefile;
};
