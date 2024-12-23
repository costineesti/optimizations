#include "../../include/tokenize/token.hpp"
#ifndef GOLDEN_NUMBER
#define GOLDEN_NUMBER 0.618033988749895
#endif

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

Token::Token(){}
Token::~Token(){}

// Function to convert TokenType enum to string for printing
std::string Token::tokenTypeToString(Token::TokenData tkData) {
    switch (tkData.type) {
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
std::vector<Token::TokenData> Token::tokenize(const std::string& expr) {
    std::vector<Token::TokenData> tokens;
    std::string temp;

    for (size_t i = 0; i < expr.size(); ++i) {
        char c = expr[i];

        // Ignore spaces
        if (std::isspace(c)) continue;

        // Check for a negative number (e.g., "-1.000000")
        if (c == '-' && (!std::isdigit(expr[i - 1]) && !std::isalpha(expr[i - 1])
        && !std::isalpha(expr[i+1]) && !std::isspace(expr[i - 1]) && expr[i - 1] != ')' && expr[i+1] != '(')) {
            temp += c;  // Start building the number with the negative sign
            ++i;  // Move to the next character
            while (i < expr.size() && (std::isdigit(expr[i]) || expr[i] == '.')) {
                temp += expr[i];
                ++i;
            }
            tokens.push_back({ Token::NUMBER, temp });
            temp.clear();
            --i;  // Adjust the index since the outer loop will increment it again
        }
        else if (c == '-' && (!std::isalpha(expr[i - 1]) && !std::isdigit(expr[i-1])
        && std::isalpha(expr[i+1]) && !std::isspace(expr[i - 1]) && expr[i - 1] != ')')) {
            temp += c;  // Start building the variable with the negative sign
            ++i;  // Move to the next character
            while (i < expr.size() && (std::isalpha(expr[i]))) {
                temp += expr[i];
                ++i;
            }
            tokens.push_back({ Token::VARIABLE, temp });
            temp.clear();
            --i;  // Adjust the index since the outer loop will increment it again
        }
        // If digit or part of a number (not negative)
        else if (std::isdigit(c) || (c == '.' && !temp.empty())) {
            temp += c;
        }
        // If not part of a number, process the token collected and then handle operators, variables, etc.
        else {
            if (!temp.empty()) {
                tokens.push_back({ Token::NUMBER, temp });
                temp.clear();
            }
            // Handle variable, parentheses, and operators
            if (c == 'x') {
                tokens.push_back({ Token::VARIABLE, "x" });
            } else if (c == 'y') {
                tokens.push_back({ Token::VARIABLE, "y" });
            } else if (c == 's') {
                tokens.push_back({ Token::VARIABLE, "s" });
            } else if (c == '(') {
                tokens.push_back({ Token::LEFT_PARAN, "(" });
            } else if (c == ')') {
                tokens.push_back({ Token::RIGHT_PARAN, ")" });
            } else if (c == '*' || c == '+' || c == '-' || c == '/' || c == '^') {
                tokens.push_back({ Token::OPERATOR, std::string(1, c) });
            }
        }
    }

    if (!temp.empty()) {
        tokens.push_back({ Token::NUMBER, temp });
    }

    return tokens;
}

// Function to get precedence of operators
int Token::getPrecedence(const std::string& op) {
    if (op == "+" || op == "-") return 1;
    if (op == "*" || op == "/") return 2;
    if (op == "^") return 3;
    return 0;
}

// Function to get associativity of operators
bool Token::isLeftAssociative(const std::string& op) {
    if (op == "^") return false;
    return true;  // All others are left associative
}

// implement Shunting Yard!
std::queue<Token::TokenData> Token::ShuntingYard(const std::vector<Token::TokenData> &tokens){
    std::queue<Token::TokenData> outputQueue;
    std::stack<Token::TokenData> operatorStack;
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
        if(token.type == Token::NUMBER || token.type == Token::VARIABLE){
            outputQueue.push(token); // PUSH NUMBERS AND VARIABLES TO queue
        }
        else if(token.type == Token::FUNCTION){
            operatorStack.push(token); // Push functions onto the stack.
        }
        else if(token.type == Token::OPERATOR){
            // precedence theory from https://en.wikipedia.org/wiki/Operators_in_C_and_C%2B%2B
            while (!operatorStack.empty() &&
                    operatorStack.top().type == Token::OPERATOR &&
                    (getPrecedence(operatorStack.top().value) > getPrecedence(token.value) ||
                    (getPrecedence(operatorStack.top().value) == getPrecedence(token.value) && isLeftAssociative(token.value))) &&
                    operatorStack.top().type != Token::LEFT_PARAN) {

                        outputQueue.push(operatorStack.top());
                        operatorStack.pop();

                    }
            operatorStack.push(token);
        }
        else if(token.type == Token::LEFT_PARAN){
            operatorStack.push(token);
        }
        else if(token.type == Token::RIGHT_PARAN){
            while (!operatorStack.empty() && operatorStack.top().type != Token::LEFT_PARAN){
                outputQueue.push(operatorStack.top());
                operatorStack.pop();
            }
            if (!operatorStack.empty() && operatorStack.top().type == Token::LEFT_PARAN){
                operatorStack.pop(); // pop the left paran and discard it.
            }
            if (!operatorStack.empty() && operatorStack.top().type == Token::FUNCTION){
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

// Evaluation of a reverse polish notation.
double Token::evaluateRPN(std::queue<Token::TokenData> outputQueue, const std::map<std::string, double>& variableValues) {
    std::stack<double> evalStack;

    while (!outputQueue.empty()) {
        Token::TokenData token = outputQueue.front();
        outputQueue.pop();

        if (token.type == Token::NUMBER) {
            evalStack.push(std::stod(token.value));
        }
        else if (token.type == Token::VARIABLE) {
            // Check if the variable is negative (starts with '-')
            if (token.value[0] == '-') {
                // Extract the variable name after the '-' and negate the value
                std::string varName = token.value.substr(1);  // Skip the first character ('-')
                evalStack.push(-variableValues.at(varName));
            } else {
                // Push the value of the variable directly
                evalStack.push(variableValues.at(token.value));
            }
        }
        else if (token.type == Token::OPERATOR) {
            double b = evalStack.top(); evalStack.pop();
            double a = evalStack.top(); evalStack.pop();
            evalStack.push(operators[token.value](a, b));
        }
        else if (token.type == Token::FUNCTION) {
            double a = evalStack.top(); evalStack.pop();
            evalStack.push(functions[token.value](a));
        }
    }

    return evalStack.top();
}


/** 
@brief: Golden section algorithm.
inputs:
    - outputQueue = the function in RPN (postorder)
    - a = lower margin
    - b = upper margin
    - e = tolerance
output:
    - function minima.
*/
std::pair<double,double> Token::golden_section(std::queue<Token::TokenData> outputQueue, double a, double b, double e, const char* variableName){
    double d = b - a;
    double x1, x2, f_x1, f_x2;
    while(b-a > e){
        d = GOLDEN_NUMBER * d;
        x1 = b - d;
        x2 = a + d;
        std::map<std::string, double> x1_map;
        std::map<std::string, double> x2_map;
        x1_map[variableName] = x1;
        x2_map[variableName] = x2; // This is where it might be a hussle; because these methods are for functions with only one variable..

        f_x1 = evaluateRPN(outputQueue, x1_map);
        f_x2 = evaluateRPN(outputQueue, x2_map);

        if (f_x1 <= f_x2){
            b = x2;
        }
        else{
            a = x1;
        }
    }
    return {a,b};
}

/**
@brief: Fibonacci Series algorithm.
inputs:
    - outputQueue = the function in RPN (postorder)
    - a = lower margin
    - b = upper margin
    - e = tolerance
output:
    - function minima.
*/
std::pair<double,double> Token::fibonacci_series(std::queue<Token::TokenData> outputQueue, double a, double b, double e, const char* variableName){
    double f1 = 2, f2 = 3, f3 = 5;
    double d, x1, x2, f_x1, f_x2;
    while(b-a > e){
        d = b-a;
        x1 = b-d*f1/f2;
        x2 = a+d*f1/f2;
        std::map<std::string, double> x1_map;
        std::map<std::string, double> x2_map;
        x1_map[variableName] = x1;
        x2_map[variableName] = x2; // This is where it might be a hussle; because these methods are for functions with only one variable..

        f_x1 = evaluateRPN(outputQueue, x1_map);
        f_x2 = evaluateRPN(outputQueue, x2_map);

        if ( f_x1 <= f_x2 ){
            b = x2;
        }
        else{
            a = x1;
        }
        f1 = f2;
        f2 = f3;
        f3 = f1+f2;
    }
    return {a,b};
}

// Helper function to replace all occurrences of a substring in a string
std::string Token::replace_all(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'xy'
    }
    return str;
}
