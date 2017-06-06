#include "parse.hpp"

#include <exception>
#include <stdlib.h>
#include <stdexcept>

std::string operators = "dDxX+-*";

void tolower(std::string& s){
    for(int i = 0; i < s.length(); i++){
        if(s[i] >= 'A' && s[i] <= 'Z'){
           s[i] = s[i] - 'A' + 'a';
        }
    }
}

bool is_op(char c){
    for(char o : operators){
        if(c == o) return true;
    }
    return false;
}


std::vector<std::string> tokenize(std::string s){
    auto first = s.begin();
    auto last = s.begin();
    std::vector<std::string> out;
    while(last != s.end()){
        if(is_op(*last)){
            out.push_back(std::string(first, last));
            out.push_back(std::string(1, *last)); //fill a string with 1 char
            last++;
            first = last;
        }
        else{
            last++;
        }
    }
    out.push_back(std::string(first, last));
    return out;
}

std::vector<Token> infix_to_postfix(std::vector<std::string> list){
    std::vector<Token> thestack;
    std::vector<Token> opstack;
    Token temp;
    try{
        for(std::string s : list){
            if(is_op(s[0])){
                if(opstack.size() > 0){
                    thestack.push_back(opstack.back());
                    opstack.pop_back();
                }
                temp.op = true;
                temp.value = s[0];
                opstack.push_back(temp);
            }
            else if(s.length() > 0){
                temp.op = false;
                temp.value = std::stoi(s);
                thestack.push_back(temp);
            }
        }
    }
    catch(...){
        std::runtime_error p("Invalid expression. Please try again.");
        throw p;
    }
    while(opstack.size() > 0){
        thestack.push_back(opstack.back());
        opstack.pop_back();
    }
    return thestack;
}

//void printTokens(std::vector<Token> in){
//    for(Token t : in){
//        if(t.op){
//            std::cout << char(t.value) << ' ';
//        }
//        else{
//            std::cout << t.value << ' ';
//        }
//    }
//    std::cout << '\n';
//}

//----------------------------------------------

SyntaxTree::SyntaxTree(std::string e):expr(e){
    //srand(time(NULL));
    root = new Node;
    tolower(expr);
    try{
        exprstack = infix_to_postfix(tokenize(expr));
    }
    catch(std::runtime_error& p){
        throw p;
    }
    build(root);
    isBuilt = true;
}

SyntaxTree::SyntaxTree(){
    root = nullptr;
    //srand(time(NULL));
    isBuilt = false;
}

void SyntaxTree::setExpr(std::string e){
    expr = e;
    freeTree(root);
    root = new Node;
    tolower(expr);
    try{
        exprstack = infix_to_postfix(tokenize(expr));
    }
    catch(std::runtime_error& p){
        throw p;
    }
    build(root);
    isBuilt = true;
}

SyntaxTree::~SyntaxTree(){
    freeTree(root);
}

void SyntaxTree::freeTree(Node* n){
    if(n == nullptr) return;
    freeTree(n->left);
    freeTree(n->right);
    delete n;
}

Node* copyTree(Node* n){
    if(n == nullptr) return nullptr;
    Node* clone = new Node;
    clone->op = n->op;
    clone->value = n->value;
    clone->left = copyTree(n->left);
    clone->right = copyTree(n->right);
    return clone;
}

void SyntaxTree::build(Node* n){
    if(exprstack.size() == 0) return;
    Token t = exprstack.back();
    exprstack.pop_back();
    n->op = t.op;
    n->value = t.value;
    if(n->op){
        n->right = new Node;
        build(n->right);
        if(exprstack.size() == 0) return;
        n->left = new Node;
        build(n->left);
    }
    else{
        return;
    }
}

int SyntaxTree::roll(int numdice, int numsides){
    int total = 0;
    for(int i = 0; i < numdice; i++){
        total += rand() % numsides + 1;
    }
    return total;
}

int SyntaxTree::evaluate(){
    return evaluate(root);
}

int SyntaxTree::evaluate(Node* n){
    if(n->left == nullptr && n->right == nullptr){
        return n->value;
    }
    char c = n->value;
    int lval, rval;
    if(c == 'd' && n->left == nullptr){
        lval = 1; //this way, users dont have to specify rolling "1d6", instead they can just roll "d6"
    }
    else{
        lval = evaluate(n->left);
    }
    rval = evaluate(n->right);
    int foo = 0;
    switch(c){
        case 'd':
            return roll(lval, rval);
        case '+':
            return lval + rval;
        case '-':
            return lval - rval;
        case 'x':
        case '*':
            return lval * rval;
    }
}

std::string SyntaxTree::str(){
    return expr;
}

//bool SyntaxTree::operator==(const SyntaxTree& rhs){ return expr == rhs.expr; }
//bool SyntaxTree::operator!=(const SyntaxTree& rhs){ return !(expr == rhs.expr); }

