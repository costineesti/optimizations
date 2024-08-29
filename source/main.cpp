#include "../include/main.hpp"

int main() {
    std::string expression;
    std::cout << "Enter a mathematical expression in terms of x and y: ";
    std::getline(std::cin, expression);

	Token tokenizer;
    
    std::vector<Token::TokenData> tokens = tokenizer.tokenize(expression);

    // Print the tokens
    for (const auto& token : tokens) {
        std::cout << "Token Type: " << tokenizer.tokenTypeToString(token) << ", Value: " << token.value << std::endl;
    }

    std::queue<Token::TokenData> outputQueue = tokenizer.ShuntingYard(tokens);

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

   double result = tokenizer.evaluateRPN(outputQueue, xValue, yValue);

   std::cout << "expression value for x1 = " << xValue << " and x2 = " << yValue<< " is: " << result << std::endl;

    return 0;
}

/*
- MAKE THE FUNCTIONS TAKE 2 VARIABLES!
- now i need to implement the optimization algorithms in different classes
- i will make them take a queue as input and a value for X.
*/