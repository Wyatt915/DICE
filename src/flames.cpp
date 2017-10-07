#include "flames.hpp"

#include <ctime>
#include <ncurses.h>
#include <panel.h>
#include <stdlib.h>

//----------------------------------------[Helper functions]----------------------------------------

static int** init(int y, int x){
    int** out = new int*[y];
    for(int i = 0; i < y; i++){
        out[i] = new int[x];
    }

    for(int i = 0; i < y; i++){
        for(int j = 0; j < x; j++){
            out[i][j] = 0;
        }
    }
    return out;
}

static void deallocate(int** in, int rows){
    for(int i = 0; i < rows; i++){
        delete[] in[i];
    }
    delete[] in;
}

static int weightedprob(int max){
    if(max == 0) return 0;
    int r = (rand() % max);
    if(r == 0) max--;
    return max;
}

static unsigned long time_in_ms(){
    return std::clock() / (double)(CLOCKS_PER_SEC / 1000);
}

static void set_colors(){
    start_color();
    init_color(COLOR_RED,      300,   0,     0);
    init_color(COLOR_GREEN,    500,   0,     0);
    init_color(COLOR_BLUE,     700,   100,   0);
    init_color(COLOR_YELLOW,   900,   300,   0);
    init_color(COLOR_MAGENTA,  1000,  500,   100);
    init_color(COLOR_CYAN,     1000,  800,   500);
    init_color(COLOR_WHITE,    1000,  1000,  1000);
    
    init_pair(1,  COLOR_RED,      COLOR_BLACK);
    init_pair(2,  COLOR_GREEN,    COLOR_BLACK);
    init_pair(3,  COLOR_BLUE,     COLOR_BLACK);
    init_pair(4,  COLOR_YELLOW,   COLOR_BLACK);
    init_pair(5,  COLOR_MAGENTA,  COLOR_BLACK);
    init_pair(6,  COLOR_CYAN,     COLOR_BLACK);
    init_pair(7,  COLOR_WHITE,    COLOR_BLACK);
}

//----------------------------------------[Member functions]----------------------------------------

Fireplace::Fireplace(){
    maxtemp = 10;
    set_colors();
    getmaxyx(stdscr, height, width);

    win = newwin(height,width, 0, 0);
    pan = new_panel(win);

    show_panel(pan);
    
    field = init(height, width);
    count = init(height, width);
    hotplate = new int[width];
    
    for(int i = 0; i < width; i++){
        hotplate[i] = rand() % 2;
    }
    timer = time_in_ms();
    framerate = 50;
}

Fireplace::~Fireplace(){
    del_panel(pan);
    delwin(win);
    deallocate(field, height);
    deallocate(count, height);
    delete[] hotplate;
}

void Fireplace::wolfram(const int rule){
    int l,c,r;
    int* next = new int[width];
    int current;
    for(int i = 0; i < width; i++){
        l = hotplate[(i-1)%width];
        c = hotplate[i];
        r = hotplate[(i+1)%width];
        current = (l << 2) | (c << 1) | r;
        next[i] = ((1 << current) & rule) > 0 ? 1 : 0;
    }

    for(int i = 0; i < width; i++){
        hotplate[i] = next[i];
    }
    delete[] next;
}

float Fireplace::hotplate_temp_at(int x){
    float total = 0;
    for(int i = -9; i <= 9; i++){
        int j = x + i;
        if(j <0 || j >= width){
            total +=0;
        }
        else{
            total += hotplate[j];
        }
    }
    total = (total * maxtemp) / 9.0;
    return total > maxtemp ? maxtemp : total;
}

void Fireplace::nextframe(){
    for(int i = 1; i <= height; i++){
        for(int j = 0; j < width; j++){
            float avg = 0;
            //int temp = rand() % maxtemp * 4;
            int counter = 0;
            for(int xoff = -3; xoff <= 3; xoff++){
                for(int yoff = -1; yoff <= 3; yoff++){
                    int y = i + yoff;
                    y = y < 0 ? 0 : y;
                    int x = j + xoff;
                    if(x < 0)         avg += 0;
                    else if(x >= width) avg += 0;
                    else if(y >= height)  avg += hotplate_temp_at(x);
                    else avg += field[y][x];
                    counter++;
                }
            }
            avg /= counter;
            count[i - 1][j] = weightedprob(avg);
        }
    }
    
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            field[i][j] = count[i][j];
        }
    }
}


void Fireplace::animate(){
    //this is a non-blocking, non threaded way to only draw the next frame
    //every [framerate] milliseconds.
    if(time_in_ms() >= timer + framerate){
        char disp;
        for(int i = 0; i < height; i++){
            for(int j = 0; j < width; j++){
                wmove(win, i,j);
                disp = field[i][j] == 0 ? ' ' : '@';
                int color = (7 * field[i][j] / maxtemp) + 1;
                wattron(win, COLOR_PAIR(color));
                waddch(win, disp);
                wattroff(win, COLOR_PAIR(color));
            }
        }
        nextframe();
        wolfram(60);
        timer += framerate;
    }
}
