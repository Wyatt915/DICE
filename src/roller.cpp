#include "roller.hpp"

#include "font.hpp"

#include <cctype>
#include <stdlib.h>
#include <time.h>
#include <iostream>

#include <vector>
#include <string>

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
			|| c == '-';	
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

std::vector<int> results(std::vector<std::string> tokens){
    std::vector<std::string> temp;
    std::vector<int> res;
    for(int i = 0; i < tokens.size(); i++){
        std::string current = tokens[i];
        temp = split(current, 'D');
        int numdice = std::stoi(temp[0]);
        size_t pos;
        int numsides = std::stoi(temp[1], &pos, 10);
        int modifiertype = -1;
        int modifier;
        if(pos < temp[1].length()){
            std::string s = temp[1];
            modifiertype = s[pos];
            temp = split(s, modifiertype);
            modifier = std::stoi(temp[1]);
        }
        int total = 0;
        for(int j = 0; j < numdice; j++){
            total += rand() % numsides + 1;
        }
        if(modifiertype > -1){
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

int get_total(std::string roll){
    sanitize(roll);
    std::vector<int> res = results(split(roll, ' '));
    int out = 0;
    for(int i : res){
        out += i;
    }
    return out;
}

//int main(int argc, char* argv[]){
//    srand(time(NULL));
//    std::string myRoll = argv[1];
//    int total = get_total(myRoll);
//    if(total < 0) total = 0;
//    font::print(total);
//    return 0;
//}

#include <ncurses.h>
#include <panel.h>

Roller::Roller(){
    srand(time(NULL));
    int h, w;
    getmaxyx(stdscr, h, w);
    created = true;
    rollwin = newwin(h/2, w/2, h/4, w/4);
    rollpanel = new_panel(rollwin);
    getmaxyx(rollwin, fieldheight, fieldwidth);
    wmove(rollwin, fieldheight - 1, 1);
    doupdate();
}

void Roller::roll(std::string command){
    int t = get_total(command);
    std::vector<std::string> pretty = font::lines(t);
    int width = pretty[0].length();
    int height = pretty.size();
    cury = (fieldheight - height)/2;
    curx = (fieldwidth - width)/2;
    wmove(rollwin, cury, curx);
    for(int i = 0; i < height; i++){
        wmove(rollwin, cury + i, curx);
        waddstr(rollwin, pretty[i].c_str());
    }
    show_panel(rollpanel);
    refresh();
    box(rollwin, 0, 0);
    wrefresh(rollwin);
}

Roller::~Roller(){
    del_panel(rollpanel);
    delwin(rollwin);
}
