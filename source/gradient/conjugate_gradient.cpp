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

// Lambda to add "+" before positive numbers in order for the parser to know in cases like (-x+y)
auto to_string_with_sign = [](double value) -> std::string {
    if (value >= 0) {
        return "+" + std::to_string(value);  // Add "+" sign for positive numbers
    } else {
        return std::to_string(value);  // No "+" for negative numbers
    }
};

double Conjugate_Gradient::solve_for_step(std::string s_expression){
    auto tokens = tokenizer.tokenize(s_expression);
    std::queue<Token::TokenData> outputQueue = tokenizer.ShuntingYard(tokens);
    AST ast;
    Node* root = ast.buildAST(outputQueue);
    Node* differential = differentiator.differentiate(root, "s");
    Node* simplified_differential = differentiator.simplify(differential);
    const std::string diff_expression = differentiator.toInfix(simplified_differential);
    std::cout << "'s' differential: " << diff_expression << std::endl;
    auto diff_postorder = differentiator.postorder(simplified_differential);
    // .... Solve for s -- what solver should I use?
    auto diff_tokens = tokenizer.tokenize(diff_postorder);
    auto diff_rpn = tokenizer.ShuntingYard(diff_tokens);

    double s_coefficient = 0.0; // Coefficient of 's' (a in a*s + b = 0)
    double constant_term = 0.0; // Constant term (b in a*s + b = 0)
    bool found_s = false;

    while (!diff_rpn.empty()) {
        // Get the front token from the queue
        Token::TokenData token = diff_rpn.front();
        diff_rpn.pop();  // Remove the token from the queue

        if (token.type == Token::VARIABLE && token.value == "s") {
            // Mark that we found the variable 's'
            found_s = true;
        } else if (token.type == Token::NUMBER) {
            if (found_s) {
                // The number before 's' is its coefficient
                s_coefficient = std::stod(token.value);
                found_s = false; // Reset the flag
            } else {
                // Any other number is treated as the constant term
                constant_term = std::stod(token.value);
            }
        }
    }

    // Step 3: Solve for 's' in the linear equation: a*s + b = 0
    if (s_coefficient != 0) {
        // Solving the equation s = -b / a
        return -constant_term / s_coefficient;
    } else {
        std::cerr << "Error: No valid coefficient for 's'." << std::endl;
        return 0.0; // Or handle error appropriately
    }
    return 1;
}

// Return the step by imposing d/ds == 0
/** @brief
 * Split F(s) into a*s^2 + b*s + c so I can solve it by using solve_deg2.
 */

int solve_deg2(double a, double b, double c, double &x1, double &x2)
{ /// based on delta and basic rules.
  double delta = b * b - 4 * a * c;

  if (delta < 0)
    return 0;

  double inv_2a = 0.5 / a;

  if (delta == 0)
  {
    x1 = -b * inv_2a;
    x2 = x1;
    return 1;
  }

  double sqrt_delta = sqrt(delta);
  x1 = (-b + sqrt_delta) * inv_2a;
  x2 = (-b - sqrt_delta) * inv_2a;
  return 2;
}

/// Reference : Eric W. Weisstein. "Cubic Equation." From MathWorld--A Wolfram Web Resource.
/// http://mathworld.wolfram.com/CubicEquation.html
/// This is for 3rd degree equations, taken from https://github.com/CL2-UWaterloo/f1tenth_ws/blob/main/src/scan_matching/src/transform.cpp

void Conjugate_Gradient::_run(){

    int k = 0;
    std::string substitute_function = m_expression;

    // Computing the initial points and initial direction.
    auto gradient = differentiator.computeJacobian(m_function, x_new);
    std::string grad_x = "("+std::to_string(-gradient(0, 0)) +"*s" + to_string_with_sign(x_curr["x"])+")";  // Gradient at x:x0 + s*dir_k 
    std::string grad_y = "("+std::to_string(-gradient(0, 1)) +"*s" + to_string_with_sign(x_curr["y"])+")";  // Gradient at y:x0 + s*dir_k
    // Replace "x" with grad_x and "y" with grad_y in the function
    substitute_function = tokenizer.replace_all(m_expression, "x", grad_x); // I want to replace variable "x" with d/dx * s
    substitute_function = tokenizer.replace_all(m_expression, "y", grad_y); // replace variable "y" with d/dy * s
    std::cout << substitute_function << std::endl;
    // Now solve for s by equalizing d/ds(substitute_function) == 0.
    this->step = solve_for_step(substitute_function);
    std::cout << "step: " << this->step << std::endl;


    // while(differentiator.norm(x_curr, x_new) > m_tolerance){

    //     x_curr = x_new;
    //     // Compute the gradient
    //     auto gradient = differentiator.computeJacobian(m_function, x_curr);

        //BETA = gradient.transpose()*gradient;


    // }
}