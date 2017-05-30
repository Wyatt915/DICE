#include "font.hpp"
#include "roller.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <vector>
#include <regex>

bool valid_expression(std::string expr){
    std::regex r("\\d*[dD]\\d+([-\\+\\*xX]\\d+)?");
    return std::regex_match(expr, r);
}

std::string trim(const std::string& str){
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first)
    {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

bool is_in_set(char c){
	return 	(c >= '0' && c <= '9')
			|| c == 'D'
			|| c == 'X'
			|| c == '+'
			|| c == '-'
            || c == '*';	
}

void sanitize(std::string& in){
	in = trim(in);
	auto it = in.begin();
	while(it != in.end()){
		*it = toupper(*it);
		it++;
	}
	it = in.begin();
	while(it != in.end()){
		if(is_in_set(*it)){
			it++;
        }
        else{
            in.erase(it);
        }
	}
}
		

std::vector<std::string> split(std::string inst, char delim){
    std::vector<std::string> tokens;
    auto fp = inst.begin(); //first position of token
    auto lp = fp;           //last position of token
    while(lp != inst.end()){
        if(*lp == delim){
            tokens.push_back(std::string(fp, lp));
            lp++;
            fp = lp;
        }
        else{
            lp++;
        }
    }
    tokens.push_back(std::string(fp, lp)); //there is one remaining after the loop exits.
    return tokens;
}

void Roller::insert(char c){
    if(c > 31 && c < 127){
        auto pos = expr.begin();
        std::advance(pos, curx - 2);
        expr.insert(pos, c);
        move_right();
    }
}
void Roller::remove(){
    auto pos = expr.begin();
    std::advance(pos, curx - 2);
    if(pos != expr.end()){
        expr.erase(pos);
    }
}

std::vector<int> results(std::vector<std::string> tokens){
    std::vector<std::string> temp;
    std::vector<int> res;
    for(int i = 0; i < tokens.size(); i++){
        std::string current = tokens[i];
        temp = split(current, 'D');
        int numdice = temp[0].length() == 0 ? 1 : std::stoi(temp[0]);
        
        char modifiertype;
        int modifier;
        std::string s = temp[1];

        std::string ops = "+-X*";
        auto o = ops.begin();
        bool hasmod = false; //if the expression has a modifier
        while(!hasmod && o != ops.end()){
            temp = split(s, *o);
            if(temp.size() > 1){
                modifiertype = *o;
                hasmod = true;
            }
            o++;
        }

        int numsides = std::stoi(temp[0]);
        
        int total = 0;
        for(int j = 0; j < numdice; j++){
            total += rand() % numsides + 1;
        }

        if(hasmod){
            modifier = std::stoi(temp[1]);
            switch(modifiertype){
                case '+':
                    total += modifier;
                    break;
                case '-':
                    total -= modifier;
                    break;
                case 'X':
                case '*':
                    total *= modifier;
                    break;
            }
        }
       
        res.push_back(total);
    }
    return res;
}

#include <ncurses.h>
#include <panel.h>

Roller::Roller(){
    srand(time(NULL));
    int h, w;
    exprpos = 0;
    histpos = 0;
    //history.push_back("");
    getmaxyx(stdscr, h, w);
    created = true;
    rollwin = newwin(h/2, w/2, h/4, w/4);
    rollpanel = new_panel(rollwin);
    getmaxyx(rollwin, fieldheight, fieldwidth);
    wmove(rollwin, fieldheight - 1, 1);
    doupdate();
    show_panel(rollpanel);
    refresh();
}

void Roller::roll(std::string command){
    wmove(rollwin, 1, 1);
    wclrtobot(rollwin);
    if(command.length() == 0) command = prevexpr;
    sanitize(command);
    std::vector<std::string> tokens = split(command, ' ');

    bool valid = true;

    for(std::string s : tokens){
        valid = valid && valid_expression(s);
    }

    if(!valid){
        wmove(rollwin, fieldheight - 3, 1);
        waddstr(rollwin, "Invalid expression. Please try again.");
        expr = "";
        update();
    }
    else{
        refresh();
        std::vector<int> res = results(tokens);
        int total = 0;
        for(int i : res){
            total += i;
        }
        if(total < 0) total = 0;
        std::vector<std::string> pretty = font::lines(total);
        int width = pretty[0].length();
        int height = pretty.size();
        cury = (fieldheight - height)/2;
        curx = (fieldwidth - width)/2;
        wmove(rollwin, cury, curx);
        for(int i = 0; i < height; i++){
            wmove(rollwin, cury + i, curx);
            waddstr(rollwin, pretty[i].c_str());
        }
        if(history.size() == 0){
            history.push_back(command);
        }
        else if(command != history.back()){
            history.push_back(command);
        }
        prevexpr = command;
    }
}

Roller::~Roller(){
    del_panel(rollpanel);
    delwin(rollwin);
}

void Roller::update(){
    wmove(rollwin, fieldheight - 2, 1);
    wclrtoeol(rollwin);
    waddch(rollwin, ':');
    waddstr(rollwin, expr.c_str());
    wmove(rollwin, fieldheight - 2, exprpos + 2);
    getyx(rollwin, cury, curx);
    box(rollwin, 0, 0);
    wrefresh(rollwin);
}

void Roller::listen(){
    int c;
    while((c = getch()) != 27){
        process(c);
    }
}

bool Roller::move_up(){
    if(histpos > 0 && history.size() > 0){
        histpos--;
        expr = history[histpos];
        move_end();
        return true;
    }
    return false;
}

bool Roller::move_down(){
    if(histpos < history.size() && history.size() > 0){
        histpos++;
        if(histpos == history.size()){
            expr = "";
        }
        else{
            expr = history[histpos];
        }
        move_end();
        return true;
    }
    return false;
}

bool Roller::move_left(){
    if(exprpos > 0){
        exprpos--;
        return true;
    }
    return false;
}

bool Roller::move_right(){
    if(exprpos < expr.length()){
        exprpos++;
        return true;
    }
    return false;
}

bool Roller::move_home(){
    exprpos = 0;
    return true;
}

bool Roller::move_end(){
    exprpos = expr.length();
    return true;
}

void Roller::process(int c){
    switch(c){
        case KEY_UP:
            move_up();
            break;
        case KEY_DOWN:
            move_down();
            break;
        case KEY_END:
            move_end();
            break;
        case KEY_HOME:
            move_home();
            break;
        case KEY_LEFT:
            move_left();
            break;
        case KEY_RIGHT:
            move_right();
            break;
        case 10:
        case 13:
        case KEY_ENTER:
            roll(expr);
            expr = "";
            move_home();
            histpos = history.size();
            break;
        case KEY_BACKSPACE:
            if(move_left()){
                update();
                remove();
            }
            break;
        case KEY_DC:
            remove();
            break;
        default:
            insert(c);
            break;
    }
    update();
}
        
        
