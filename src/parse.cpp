/*
 *  DICE source file
 *  parse.cpp
 *  
 */

#include "parse.hpp"
#include "utils.hpp"

#include <stdlib.h>
#include <stdexcept>
#include <ctype.h>

std::string operators = "dDxX+-*/@()";

std::vector<std::string> reserved_words = { "dx", "iq", "ht", "st", "hp", "will", "per", "fp" };

//---------------------------------------------[Token]---------------------------------------------

int precedence(Token t){
    char c = t.value;
    switch(c){
        case '+':
        case '-':
            return 0;
        case 'x':
        case 'X':
        case '*':
        case '/':
            return 1;
        case 'd':
        case 'D':
        case '@':
            return 2;
    }
}

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

Token TokenStack::peek(){
    return data[top];
}

void TokenStack::push(Token t){
    top++;
    data[top] = t;
    numTokens++;
}

Token TokenStack::pop(){
    if(numTokens <= 0){
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
    auto first = std::begin(s);
    auto last = std::begin(s);
    std::vector<Token> out;
    enum tokentype{ token_null, token_op, token_num} prevToken;
    prevToken = token_null;
    while(first != std::end(s)){
        if(isdigit(*first)){
            last = first;
            do{
                last++;
            } while(last != std::end(s) && isdigit(*last));

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
            } while(last != std::end(s) && isdigit(*last));
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
        int i = 0;
        while(i < list.size()){
            Token t = list[i];
            //Case 1: Push operands as they arrive. This is the only case
            //involving operands.
            if(!t.op){
                postfix.push(t);
                i++;
            }
            //Case 2: If the stack is empty or contains a left parenthesis on
            //top, push the incoming operator onto the stack.
            else if (opstack.is_empty() || opstack.peek().value == '('){
                opstack.push(t);
                i++;
            }
            //Case 3: If the incoming symbol is a left parenthesis, push it on
            //the stack.
            else if (t.value == '('){
                opstack.push(t);
                i++;
            }
            //Case 4: If the incoming symbol is a right parenthesis, pop the
            //stack and print the operators until you see a left parenthesis.
            else if (t.value == ')'){
                while(opstack.peek().value != '('){
                    postfix.push(opstack.pop());
                }
                opstack.pop();//discard parentheses
                i++;
            }
            //Case 5: If the incoming symbol has higher precedence than the
            //top of the stack, push it on the stack.
            else if (precedence(t) > precedence(opstack.peek())){
                opstack.push(t);
                i++;
            }
            //Case 6: If the incoming symbol has equal precedence with the
            //top of the stack, pop opstack and push it to postfix and then
            //push the incoming operator.
            else if (precedence(t) == precedence(opstack.peek())){
                postfix.push(opstack.pop());
                opstack.push(t);
                i++;
            }
            //Case 7: If the incoming symbol has lower precedence than the
            //symbol on the top of the stack, pop the stack and push it to
            //postfix. Then test the incoming operator against the new
            //top of stack.
            else{
                postfix.push(opstack.pop());
                //Don't increment
            }
        }

        //There should be no parentheses left.
        while(opstack.not_empty()){
            postfix.push(opstack.pop());
            Token temp = postfix.peek();
            if(temp.op && (temp.value == '(' || temp.value == ')')){
                throw std::runtime_error("Mismatched Parentheses");
            }
        }
    }
    catch(std::runtime_error& e){
        throw e;
    }
    catch(...){
        throw std::runtime_error("Invalid expression. Please try again.");
    }
    return postfix;
}

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
        case '@':
            return lval - rval;
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
