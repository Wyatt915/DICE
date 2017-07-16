#pragma once

#include <string>
#include <vector>

struct ParseException : public std::exception {
   const char * what () const throw () {
      return "Parsing exception";
   }
};

struct Node{
    bool op;
    int value = 0;
    Node* left = nullptr;
    Node* right = nullptr;
};

struct Token{
    bool op = false;
    int value = 0;
};

class SyntaxTree{
    public:    
        SyntaxTree(std::string e);
        SyntaxTree();
        SyntaxTree(const SyntaxTree&);
        int evaluate();
        std::string str();
        void setExpr(std::string);
        bool operator==(const SyntaxTree& b){ return expr == b.expr; }
        bool operator!=(const SyntaxTree& b){ return expr != b.expr; }
        SyntaxTree& operator=(const SyntaxTree&);
        ~SyntaxTree();
    private:
        Node* root;
        std::string expr;
        bool isBuilt;
        std::vector<Token> exprstack;
        void build(Node*);
        int evaluate(Node*);
        int roll(int, int);
};

