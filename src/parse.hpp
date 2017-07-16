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
    Token(bool o, int v):op(o), value(v){}
    Token():op(false), value(0){}
    bool op;
    int value;
};

class TokenStack{
    public:
        TokenStack();
        TokenStack(const TokenStack&);
        TokenStack& operator=(const TokenStack&);
        ~TokenStack();
        void push(Token);
        Token pop();
        int size();
        bool is_empty();
        bool not_empty();
    private:
        Token* data;
        int numTokens;
        int top;
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
        TokenStack exprstack;
        void build(Node*);
        int evaluate(Node*);
        int roll(int, int);
};


//////////////////////////////////////////////////////
//                                                  //
//      oooooooooo.    o8o                          //
//      `888'   `Y8b   `"'                          //
//       888      888 oooo   .ooooo.   .ooooo.      //
//       888      888 `888  d88' `"Y8 d88' `88b     //
//       888      888  888  888       888ooo888     //
//       888     d88'  888  888   .o8 888    .o     //
//      o888bood8P'   o888o `Y8bod8P' `Y8bod8P'     //
//                                                  //
//////////////////////////////////////////////////////
