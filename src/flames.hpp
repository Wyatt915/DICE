#pragma once

typedef struct _win_st WINDOW;
typedef struct panel PANEL;

class Fireplace{
    public:
        Fireplace();
        void animate();
        float hotplate_temp_at(int);
        void nextframe();
        void wolfram(const int);
        ~Fireplace();
    private:
        int width, height, maxtemp, framerate;
        unsigned long timer;
        int** field;
        int** count;
        int* hotplate;
        WINDOW* win;
        PANEL* pan;
};
