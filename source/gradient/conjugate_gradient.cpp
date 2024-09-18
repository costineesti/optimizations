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


double Conjugate_Gradient::solve_for_step_2nd_order(std::string& s_expression) {
    // Tokenize the expression and convert it to RPN (Reverse Polish Notation)
    auto tokens = tokenizer.tokenize(s_expression);
    auto rpnQueue = tokenizer.ShuntingYard(tokens);

    // a*s^2 + b*s + c = 0 => s = -b/2a (where the differential equals 0).
    double c = tokenizer.evaluateRPN(rpnQueue, {{"s", 0}});
    double a_plus_b = tokenizer.evaluateRPN(rpnQueue, {{"s", 1}}) - c;
    double a_minus_b = tokenizer.evaluateRPN(rpnQueue, {{"s", -1}}) - c;
    double a = (a_plus_b + a_minus_b)/2;
    double b = a_plus_b - a;
    std::cout << "a: " << a << " b: " << b << " c: " << c << "\n";

    return -b/2/a;
}


void Conjugate_Gradient::_run(){
    int k = 0;
    std::string substitute_function = m_expression;

    while(differentiator.norm(x_curr, x_new) > m_tolerance){

      x_curr = x_new;
      // Computing the initial points and initial direction.
      auto gradient = differentiator.computeJacobian(m_function, x_new);
      std::string grad_x = "("+std::to_string(-gradient(0, 0)) +"*s" + to_string_with_sign(x_curr["x"])+")";  // Gradient at x:x0 + s*dir_k 
      std::string grad_y = "("+std::to_string(-gradient(0, 1)) +"*s" + to_string_with_sign(x_curr["y"])+")";  // Gradient at y:x0 + s*dir_k
      // Replace "x" with grad_x and "y" with grad_y in the function
      substitute_function = tokenizer.replace_all(m_expression, "x", grad_x); // I want to replace variable "x" with d/dx * s
      substitute_function = tokenizer.replace_all(m_expression, "y", grad_y); // replace variable "y" with d/dy * s
      std::cout << substitute_function << "\n";
      // Now solve for s by equalizing d/ds(substitute_function) == 0.
      this->step = solve_for_step_2nd_order(substitute_function);
      std::cout << "step: " << this->step << std::endl;
    } // end while
    
}
