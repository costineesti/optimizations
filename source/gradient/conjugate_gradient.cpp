#include "../../include/gradient/conjugate_gradient.hpp"

/** @brief
 * Constructor for Conjugate_Gradient class
 * @param function as a Node* is the mathematical expression in AST form,
 * @param expression as a string is the literal mathematical expression parsed by the user,
 * @param x0 as a map in order to get the values for first guess,
 * @param tolerance as a double to always check if we reached the minimum point.
 */
Conjugate_Gradient::Conjugate_Gradient(
    Node* function,
    std::string expression,
    std::map<std::string, double> x0,
    double tolerance
)
: m_function(function)
, m_expression(expression)
, x_curr{{"x", 0.0}, {"y", 0.0}}
, m_tolerance(tolerance)
, x_new(x0)
, BETA(0.0)
, step(0.0)
{}

Conjugate_Gradient::~Conjugate_Gradient(){}

double norm(const std::map<std::string, double>& point1, const std::map<std::string, double>& point2) {
    // Compute the differences between the x and y coordinates
    double dx = point2.at("x") - point1.at("x");
    double dy = point2.at("y") - point1.at("y");

    // Compute and return the Euclidean norm (distance)
    return std::sqrt(std::pow(dx, 2) + std::pow(dy, 2));
    }

// Lambda to add "+" before positive numbers in order for the parser to know in cases like (-x+y)
auto to_string_with_sign = [](double value) -> std::string {
    if (value >= 0) {
        return "+" + std::to_string(value);  // Add "+" sign for positive numbers
    } else {
        return std::to_string(value);  // No "+" for negative numbers
    }
};

// Helper function to replace all occurrences of a substring in a string
std::string replace_all(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'xy'
    }
    return str;
}

double Conjugate_Gradient::solve_for_step(std::string s_expression){
    auto tokens = tokenizer.tokenize(s_expression);
    std::queue<Token::TokenData> outputQueue = tokenizer.ShuntingYard(tokens);
    AST ast;
    Node* root = ast.buildAST(outputQueue);
    Node* differential = differentiator.differentiate(root, "s");
    // .... Solve for s
}

// Return the step by imposing d/ds == 0

void Conjugate_Gradient::_run(){

    int k = 0;
    std::string substitute_function = m_expression;

    // Computing the initial points and initial direction.
    auto gradient = differentiator.computeJacobian(m_function, x_new);
    std::string grad_x = "("+std::to_string(-gradient(0, 0)) +"*s" + to_string_with_sign(x_curr["x"])+")";  // Gradient at x:x0 + s*dir_k 
    std::string grad_y = "("+std::to_string(-gradient(0, 1)) +"*s" + to_string_with_sign(x_curr["y"])+")";  // Gradient at y:x0 + s*dir_k
    // Replace "x" with grad_x and "y" with grad_y in the function
    substitute_function = replace_all(m_expression, "x", grad_x); // I want to replace variable "x" with d/dx * s
    substitute_function = replace_all(m_expression, "y", grad_y); // replace variable "y" with d/dy * s
    // Now solve for s by equalizing d/ds(substitute_function) == 0.
    double pas = solve_for_step(substitute_function);


    while(norm(x_curr, x_new) > m_tolerance){

        x_curr = x_new;
        // Compute the gradient
        auto gradient = differentiator.computeJacobian(m_function, x_curr);

        //BETA = gradient.transpose()*gradient;


    }
}