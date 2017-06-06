#pragma once

#include <string>
#include <vector>

class Item{
    public:
        std::string disp();
        void select();
    protected:
        std::vector<std::string> types;
        std::vector<std::string> values;
};
