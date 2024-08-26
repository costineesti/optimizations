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

int main() {
    std::string expression;
    std::cout << "Enter a mathematical expression in terms of x: ";
    std::getline(std::cin, expression);

    std::vector<Token> tokens = tokenize(expression);

    // Print the tokens
    for (const auto& token : tokens) {
        std::cout << "Token Type: " << tokenTypeToString(token.type) << ", Value: " << token.value << std::endl;
    }

    return 0;
}
