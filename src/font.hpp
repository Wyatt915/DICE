#pragma once

#include <vector>
#include <string>
#include <iostream>

std::vector<std::vector<std::string> > bigdigits {
    {
        "  .oooo.   ",
        " d8P'`Y8b  ",
        "888    888 ",
        "888    888 ",
        "888    888 ",
        "`88b  d88' ",
        " `Y8bd8P'  "
    },
    {
        "  .o  ",
        "o888  ",
        " 888  ",
        " 888  ",
        " 888  ",
        " 888  ",
        "o888o "
    },
    {
        "  .oooo.   ",
        ".dP\"\"Y88b  ",
        "      ]8P' ",
        "    .d8P'  ",
        "  .dP'     ",
        ".oP     .o ",
        "8888888888 "
    },
    {
        "  .oooo.   ",
        ".dP\"\"Y88b  ",
        "      ]8P' ",
        "    <88b.  ",
        "     `88b. ",
        "o.   .88P  ",
        "`8bd88P'   "
    },
    {
        "      .o   ",
        "    .d88   ",
        "  .d'888   ",
        ".d'  888   ",
        "88ooo888oo ",
        "     888   ",
        "    o888o  "
    },
    {
        "  oooooooo ",
        " dP\"\"\"\"\"\"\" ",
        "d88888b.   ",
        "    `Y88b  ",
        "      ]88  ",
        "o.   .88P  ",
        "`8bd88P'   "
    },
    {
        "    .ooo   ",
        "  .88'     ",
        " d88'      ",
        "d888P\"Ybo. ",
        "Y88[   ]88 ",
        "`Y88   88P ",
        " `88bod8'  "
    },
    {
        " ooooooooo ",
        "d\"\"\"\"\"\"\"8' ",
        "      .8'  ",
        "     .8'   ",
        "    .8'    ",
        "   .8'     ",
        "  .8'      "
    },
    {
        " .ooooo.   ",
        "d88'   `8. ",
        "Y88..  .8' ",
        " `88888b.  ",
        ".8'  ``88b ",
        "`8.   .88P ",
        " `boood8'  "
    },
    {
        " .ooooo.   ",
        "888' `Y88. ",
        "888    888 ",
        " `Vbood888 ",
        "      888' ",
        "    .88P'  ",
        "  .oP'     "
    }
};


namespace font{
    void print(int in){
        std::vector<std::string> out;
        std::string strval = std::to_string(in);
        for(int i = 0; i < 7; i ++){
            for(int j  = 0; j < strval.length(); j++){
                int digit = strval[j] - '0';
                std::cout << bigdigits[digit][i];
            }
            std::cout << '\n';
        }
    }
    
    std::vector<std::string> lines(int in){
        std::vector<std::string> out;
        std::string strval = std::to_string(in);
        std::string temp = "";
        for(int i = 0; i < 7; i ++){
            for(int j  = 0; j < strval.length(); j++){
                int digit = strval[j] - '0';
                temp += bigdigits[digit][i];
            }
            out.push_back(temp);
            temp = "";
        }
        return out;
    }

}
