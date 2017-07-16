#include "parse.hpp"
#include "utils.hpp"

#include <stdlib.h>
#include <stdexcept>
#include <ctype.h>

std::string operators = "dDxX+-*/@";

std::vector<std::string> reserved_words = { "dx", "iq", "ht", "st", "hp", "will", "per", "fp" };

//-------------------------------------------[TokenStack]-------------------------------------------

TokenStack::TokenStack():numTokens(0), top(-1){
    data = new Token[1024];
}

TokenStack::TokenStack(const TokenStack& other){
    numTokens = other.numTokens;
    top = other.top;
    data = new Token[1024];
    std::copy(other.data, other.data + numTokens, data);
}

TokenStack& TokenStack::operator=(const TokenStack& other){
    if(data){ delete data; }
    numTokens = other.numTokens;
    top = other.top;
    data = new Token[1024];
    std::copy(other.data, other.data + numTokens, data);
    return *this;
}

TokenStack::~TokenStack(){
    delete data;
}

int TokenStack::size(){
    return numTokens;
}

bool TokenStack::is_empty(){
    return numTokens <= 0;
}

bool TokenStack::not_empty(){
    return numTokens > 0;
}

void TokenStack::push(Token t){
    top++;
    data[top] = t;
    numTokens++;
}

Token TokenStack::pop(){
    if(numTokens == 0){
        throw std::runtime_error("Error: attempt to pop empty stack");
    }
    Token out = data[top];
    top--;
    numTokens--;
    return out;
}

//------------------------------------------[Tokenization]------------------------------------------

void expand_reserved(std::string& s){
    tolower(s);
    int rwlen = max_length_in_list(reserved_words);
    auto f = std::begin(s); //first letter in reserved word
    auto l = f;             //one after last letter in reserved word
    while(f != std::end(s)){
        for(int i = 0; i < rwlen && l != std::end(s); i++){
            l++;
            if (is_in_list(std::string(f, l), reserved_words)){
                s.replace(f,l,"10");    //TODO: actually look up the relevant values
                l--;
                f = l;
                break;
            }
        }
        f++;
        l = f;
    }
}

std::vector<Token> tokenize(std::string s){
    expand_reserved(s);
    auto first = s.begin();
    auto last = s.begin();
    std::vector<Token> out;
    enum tokentype{ token_null, token_op, token_num} prevToken;
    prevToken = token_null;
    while(first != s.end()){
        if(isdigit(*first)){
            last = first;
            do{
                last++;
            } while(last != s.end() && isdigit(*last));

            out.push_back(Token(false, std::stoi(std::string(first, last))));
            first = last;
            prevToken = token_num;
        }
        //Handle the case of negative numbers. A '-' char indicates a
        //negative number if it comes at the beginning of the string,
        //or if it follows a previous operator.
        if(*first == '-' && prevToken != token_num){
            first = last;
            do{
                last++;
            } while(last != s.end() && isdigit(*last));
            out.push_back(Token(false, std::stoi(std::string(first, last))));
            first = last;
            prevToken = token_num;
        }

        if(is_in_list(*first, operators)){
            out.push_back(Token(true, *first));
            first++;
            last = first;
            prevToken = token_op;
        }
    }
    return out;
}


TokenStack infix_to_postfix(std::vector<Token> list){
    TokenStack postfix;
    TokenStack opstack;
    try{
        for(Token t : list){
            if(t.op){
                if(opstack.size() > 0){
                    postfix.push(opstack.pop());
                }
                opstack.push(t);
            }
            else{
                postfix.push(t);
            }
        }
    }
    catch(...){
        throw std::runtime_error("Invalid expression. Please try again.");
    }
    while(opstack.not_empty()){
        postfix.push(opstack.pop());
    }
    return postfix;
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

//----------------------------------------[Tree operations]----------------------------------------

void freeTree(Node* n){
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

//-------------------------------[SyntaxTree Constructors/Destructor]-------------------------------

SyntaxTree::SyntaxTree(std::string e):expr(e){
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
    isBuilt = false;
}

SyntaxTree::SyntaxTree(const SyntaxTree& other){
    root = copyTree(other.root);
    isBuilt = other.isBuilt;
    expr = other.expr;
}

SyntaxTree& SyntaxTree::operator=(const SyntaxTree& other){
    root = copyTree(other.root);
    isBuilt = other.isBuilt;
    expr = other.expr;
    return *this;
}

SyntaxTree::~SyntaxTree(){
    freeTree(root);
}

//----------------------------------[SyntaxTree Member Functions]----------------------------------

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

void SyntaxTree::build(Node* n){
    if(exprstack.size() == 0) return;
    Token t = exprstack.pop();
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
        //an operand will have two null children
        if(!n->op){
            return n->value;
        }
        else{
            throw std::runtime_error("Invalid Expression. Please try again.");
        }
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
    
    switch (c){
        case 'd':
            return roll(lval, rval);
        case '+':
            return lval + rval;
        case '-':
            return lval - rval;
        case 'x':
        case '*':
            return lval * rval;
        case '/':
            return lval / rval;
    }
}

std::string SyntaxTree::str(){
    return expr;
}

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
