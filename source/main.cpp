#include "../include/main.hpp"

int main() {
    std::string expression;
    std::cout << "Enter a mathematical expression in terms of x and y: ";
    std::getline(std::cin, expression);

	Token tokenizer;
    
    auto tokens = tokenizer.tokenize(expression);

    // Print the tokens
    for (const auto& token : tokens) {
        std::cout << "Token Type: " << tokenizer.tokenTypeToString(token) << ", Value: " << token.value << std::endl;
    }

    std::queue<Token::TokenData> outputQueue = tokenizer.ShuntingYard(tokens);

    AST ast;
    Node* root = ast.buildAST(outputQueue);
    // Output the AST in post order ( which should be the same as RPN )
    std::cout << "The postorder from the syntax tree: " << ast.postorder(root) << std::endl;

    
    // Print the outputQueue => We can expect it in Reverse Polish Notation (RPN)
    std::cout << "The postorder from the Shunting Yard Algorithm: ";
    auto replacement = outputQueue;
    while(!replacement.empty()){
        const Token::TokenData& token = replacement.front();
        std::cout << token.value << " ";
        replacement.pop(); 
    }
    std::cout << std::endl;
    
    Differentiator diff;
    Node* differential = diff.differentiate(root, "x");
    Node* simplified_differential = diff.simplify(differential);
    std::cout << "The differential w.r.t x: " << diff.toInfix(simplified_differential) << std::endl;
    std::cout << "Differential postorder: " << diff.postorder(differential) << std::endl;
    /*

    std::cout << "Golden section minima: ";
    std::cout << tokenizer.golden_section(outputQueue, -3, 0, 0.01);
    std::cout << std::endl;

    std::cout << "Fibonacci series minima: ";
    std::cout << tokenizer.fibonacci_series(outputQueue, -3, 0, 0.01);
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
- now i need to implement the binary tree in order to compute the differential equations.
*/