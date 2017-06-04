#pragma once

typedef struct _win_st WINDOW;
typedef struct panel PANEL;
namespace sqlite {class database;}

class DiceWindow{
    public:
        DiceWindow();
        DiceWindow(WINDOW*);
        DiceWindow(WINDOW*, std::string);
        DiceWindow(std::string);
        void process(int);   //interpret keyboard input
        bool move_up();     //Cursor movement functions
        bool move_down();
        bool move_left();
        bool move_right();
        bool move_home();
        bool move_end();
        void update();      //Refresh the window, etc.
        void init();        //Helper for the constructors
        void listen();      //wait for input
        void insert(char);  //insert into the text buffer
        void remove();      //delete char under the cursor
        void clear();       //clears the buffer
        void show();
        void hide();
        void setTitle(std::string);
        ~DiceWindow();
    protected:
        sqlite::database *savefile;
        bool created;
        bool edited;
        int margin;
        int mode;
        int fieldheight;
        int fieldwidth;
        int scroll;
        int curx;
        int cury;
        std::vector<std::string> buffer;
        std::string title;
        WINDOW* dwin;
        PANEL* dpanel;        
};
