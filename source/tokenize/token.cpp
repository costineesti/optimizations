#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <cmath>
#include <map>
#include <functional>
#include <math.h>
#include <stack>  // For operations
#include <queue>  // For queue
#include <array> //  For tokens.

enum TokenType { NUMBER, VARIABLE, OPERATOR, FUNCTION, LEFT_PARAN, RIGHT_PARAN };

struct Token {
    TokenType type;
    std::string value;
};

// Define map for basic operators
std::map<std::string, std::function<double(double, double)>> operators = {
    {"+", [](double a, double b) { return a + b; }},
    {"-", [](double a, double b) { return a - b; }},
    {"*", [](double a, double b) { return a * b; }},
    {"/", [](double a, double b) { return a / b; }},
    {"^", [](double a, double b) { return std::pow(a, b); }},
};

std::map<std::string, std::function<double(double)>> functions = {
    {"sin", [](double a) { return std::sin(a); }},
    {"cos", [](double a) { return std::cos(a); }},
    {"tan", [](double a) { return std::tan(a); }},
    {"log", [](double a) { return std::log(a); }},
    {"exp", [](double a) { return std::exp(a); }},
};

// Function to convert TokenType enum to string for printing
std::string tokenTypeToString(TokenType type) {
    switch (type) {
    case NUMBER: return "NUMBER";
    case VARIABLE: return "VARIABLE";
    case OPERATOR: return "OPERATOR";
    case FUNCTION: return "FUNCTION";
    case LEFT_PARAN: return "LEFT_PARAN";
    case RIGHT_PARAN: return "RIGHT_PARAN";
    default: return "UNKNOWN";
    }
}

// Function to tokenize the input string
std::vector<Token> tokenize(const std::string& expr) {
    std::vector<Token> tokens;
    std::string temp;

    for (size_t i = 0; i < expr.size(); i++) {
        char c = expr[i];
        // if it is space
        if (std::isspace(c)) continue;

        // if number
        if (std::isdigit(c) || (c == '.' && !temp.empty())) {
            temp += c;
        }
        else {
            if (!temp.empty()) {
                tokens.push_back({ NUMBER, temp });
                temp.clear();
            }
            // if variable
            if (c == 'x') {
                tokens.push_back({ VARIABLE, "x" });
            }
            else if (c == 'y') {
                tokens.push_back({ VARIABLE, "y" });
            }
            // if paran
            else if (c == '(') {
                tokens.push_back({ LEFT_PARAN, "(" });
            }
            else if (c == ')') {
                tokens.push_back({ RIGHT_PARAN, ")" });
            }
            // if operator
            else if (c == '*' || c == '+' || c == '-' || c == '/' || c == '^') {
                tokens.push_back({ OPERATOR, std::string(1, c) });
            }
            // if function
            else if (std::isalpha(c)) {
                while (i < expr.size() && std::isalpha(expr[i])) {
                    temp += expr[i];
                    i++;
                }
                tokens.push_back({ FUNCTION, temp });
                temp.clear();
                i--; // Decrement i because it will be incremented in the loop
            }
        }
    }
    if (!temp.empty())
        tokens.push_back({ NUMBER, temp }); // if anything is left, then save before returning the tokens.
    return tokens;
}

// Function to get precedence of operators
int getPrecedence(const std::string& op) {
    if (op == "+" || op == "-") return 1;
    if (op == "*" || op == "/") return 2;
    if (op == "^") return 3;
    return 0;
}

// Function to get associativity of operators
bool isLeftAssociative(const std::string& op) {
    if (op == "^") return false;
    return true;  // All others are left associative
}

// implement Shunting Yard!
std::queue<Token> ShuntingYard(const std::vector<Token> &tokens){
    std::queue<Token> outputQueue;
    std::stack<Token> operatorStack;
/*
  While there are tokens to be read:
        Read a token
        If it's a number add it to queue
        If it's an operator
               While there's an operator on the top of the stack with greater precedence:
                       Pop operators from the stack onto the output queue
               Push the current operator onto the stack
        If it's a left bracket push it onto the stack
        If it's a right bracket 
            While there's not a left bracket at the top of the stack:
                     Pop operators from the stack onto the output queue.
             Pop the left bracket from the stack and discard it
 While there are operators on the stack, pop them to the queue
*/
    for (const auto& token : tokens){
        if(token.type == NUMBER || token.type == VARIABLE){
            outputQueue.push(token); // PUSH NUMBERS AND VARIABLES TO queue
        }
        else if(token.type == FUNCTION){
            operatorStack.push(token); // Push functions onto the stack.
        }
        else if(token.type == OPERATOR){
            // precedence theory from https://en.wikipedia.org/wiki/Operators_in_C_and_C%2B%2B
            while (!operatorStack.empty() &&
                    operatorStack.top().type == OPERATOR &&
                    (getPrecedence(operatorStack.top().value) > getPrecedence(token.value) ||
                    (getPrecedence(operatorStack.top().value) == getPrecedence(token.value) && isLeftAssociative(token.value))) &&
                    operatorStack.top().type != LEFT_PARAN) {

                        outputQueue.push(operatorStack.top());
                        operatorStack.pop();

                    }
                operatorStack.push(token);
        }
        else if(token.type == LEFT_PARAN){
            operatorStack.push(token);
        }
        else if(token.type == RIGHT_PARAN){
            while (!operatorStack.empty() && operatorStack.top().type != LEFT_PARAN){
                outputQueue.push(operatorStack.top());
                operatorStack.pop();
            }
            if (!operatorStack.empty() && operatorStack.top().type == LEFT_PARAN){
                operatorStack.pop(); // pop the left paran and discard it.
            }
            if (!operatorStack.empty() && operatorStack.top().type == FUNCTION){
                outputQueue.push(operatorStack.top()); // push the function to the output queue.
                operatorStack.pop();
            }
        }
    }

    // After the loop, pop all operators from the stack to the output queue
    while (!operatorStack.empty()){
        outputQueue.push(operatorStack.top());
        operatorStack.pop();
    }

    return outputQueue;
}

double evaluateRPN(std::queue<Token> &outputQueue, double xValue, double yValue){
    // evaluate the RPN with a given value for X
    std::stack<double> evalStack;

    while(!outputQueue.empty()){
        Token token = outputQueue.front(); // LIFO
        outputQueue.pop(); // remove the last element

        if (token.type == NUMBER) {
            evalStack.push(std::stod(token.value));
        }
        else if (token.type == VARIABLE) {
            if (token.value == "x"){
                evalStack.push(xValue);
            }
            else {evalStack.push(yValue);}
        }
        else if (token.type == OPERATOR) {
            double b = evalStack.top(); evalStack.pop(); // take b
            double a = evalStack.top(); evalStack.pop(); // take a
            evalStack.push(operators[token.value](a,b));
        }
        else if (token.type == FUNCTION) {
            double a = evalStack.top(); evalStack.pop();
            evalStack.push(functions[token.value](a));
        }
    }

    return evalStack.top();
}


int main() {
    std::string expression;
    std::cout << "Enter a mathematical expression in terms of x and y: ";
    std::getline(std::cin, expression);

    std::vector<Token> tokens = tokenize(expression);

    // Print the tokens
    for (const auto& token : tokens) {
        std::cout << "Token Type: " << tokenTypeToString(token.type) << ", Value: " << token.value << std::endl;
    }

    std::queue<Token> outputQueue = ShuntingYard(tokens);

    /*
    // Print the outputQueue => We can expect it in Reverse Polish Notation (RPN)
    while(!outputQueue.empty()){
        const Token& token = outputQueue.front();
        std::cout << token.value << " ";
        outputQueue.pop(); 
    }
    std::cout << std::endl;
    */

   double xValue, yValue;
   std::cout << "Enter the value of x: ";
   std::cin >> xValue;

   std::cout << "Enter the value of y: ";
   std::cin >> yValue;

   double result = evaluateRPN(outputQueue, xValue, yValue);

   std::cout << "expression value for x1 = " << xValue << " and x2 = " << yValue<< " is: " << result << std::endl;

    return 0;
}

/*
- MAKE THE FUNCTIONS TAKE 2 VARIABLES!
- now i need to implement the optimization algorithms in different classes
- i will make them take a queue as input and a value for X.
*/