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
    const std::string diff_expression = diff.toInfix(simplified_differential);
    auto diff_postorder = diff.postorder(simplified_differential);
    std::cout << "Differential postorder: " << diff_postorder << std::endl;
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

    std::map<std::string, double> x0;
    x0["x"] = xValue; x0["y"] = yValue;

    double result = tokenizer.evaluateRPN(outputQueue, x0);
    // Get value for differential
    auto tokens_differential = tokenizer.tokenize(diff_expression);
    auto outputQueue_differential = tokenizer.ShuntingYard(tokens_differential);
    double result_diff = tokenizer.evaluateRPN(outputQueue_differential, x0);

    std::cout << "expression value for x1 = " << xValue << " and x2 = " << yValue<< " is: " << result << std::endl;
    std::cout << "1st order differential value for x1 = " << xValue << " and x2 = " << yValue << " is: " << result_diff << std::endl;

    Newton newton(root, 0.001, x0);
    std::cout << "The interval found by Newton method: " << std::endl;
    newton._run();


    return 0;
}

/*
- now i need to implement the binary tree in order to compute the differential equations.
*/