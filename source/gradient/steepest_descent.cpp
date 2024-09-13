#include "../../include/gradient/steepest_descent.hpp"

/** @brief
 * Class constructor
 * @param function - the function in tree form,
 * @param tolerance - the tolerance we set in order to tell when we've reachef the final point,
 * @param a - lower bound,
 * @param b - upper bound,
 * @param x0 - the starting point.
 */
Steepest_Descent::Steepest_Descent(
    Node* function,
    std::string expression,
    double tolerance,
    double a,
    double b,
    std::map<std::string, double> x0
)
: m_function(function)
, m_expression(expression)
, m_tolerance(tolerance)
, a(a)
, b(b)
, d(b-a)
, x_curr{{"x", 0.0}, {"y", 0.0}}
, x_new(x0)
{}

Steepest_Descent::~Steepest_Descent(){}

// Lambda to add "+" before positive numbers in order for the parser to know in cases like (-x+y)
auto to_string_with_sign = [](double value) -> std::string {
    if (value >= 0) {
        return "+" + std::to_string(value);  // Add "+" sign for positive numbers
    } else {
        return std::to_string(value);  // No "+" for negative numbers
    }
};

void Steepest_Descent::_run(){
    unsigned int k = 0;
    std::string function_str = m_expression;
    
    while(differentiator.norm(x_curr, x_new) > m_tolerance){
        x_curr = x_new;
        auto gradient = this->differentiator.computeJacobian(m_function, x_curr);

        std::string grad_x = "("+std::to_string(-gradient(0, 0)) +"*s" + to_string_with_sign(x_curr["x"])+")";  // Gradient at x
        std::string grad_y = "("+std::to_string(-gradient(0, 1)) +"*s" + to_string_with_sign(x_curr["y"])+")";  // Gradient at y
        // Replace "x" with grad_x and "y" with grad_y in the function
        function_str = tokenizer.replace_all(m_expression, "x", grad_x); // I want to replace variable "x" with d/dx * s
        function_str = tokenizer.replace_all(m_expression, "y", grad_y); // replace variable "y" with d/dy * s
        auto tokens = this->tokenizer.tokenize(function_str);
        auto outputQueue = this->tokenizer.ShuntingYard(tokens);
        auto result = tokenizer.golden_section(outputQueue, this->a, this->b, this->m_tolerance/10, "s");
        this->a = result.first; this->b = result.second;
        step = (this->a + this->b)/2;
        x_new["x"] = -gradient(0,0)*step + x_curr["x"];
        x_new["y"] = -gradient(0,1)*step + x_curr["y"];
        k++;
    }
    double final_a = x_new["x"];
    double final_b = x_new["y"];
    std::cout<< "The steepest descent found interval in "<< k <<" steps is a: "<<final_a<<" and b: "<<final_b<< std::endl;
}