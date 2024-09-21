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
, BETA_Fletcher_Reeves(0.0)
, BETA_Polak_Ribiere(0.0)
, step(0.0)
, a(0)
, b(10)
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

    return -b/2/a;
}

/** @brief Conjugate Gradient Solver using BETA computing by Fletcher-Reeves method */
void Conjugate_Gradient::Solver_Fletcher_Reeves(){
    // initialize local variables.
    std::map<std::string, double> x_curr_FR = this->x_curr; 
    std::map<std::string, double> x_new_FR = this->x_new;
    Eigen::MatrixXd dir_k_local = this->dir_k;
    Eigen::MatrixXd d_new_local = this->d_new;
    Eigen::MatrixXd d_old_local = this->d_old;
    double final_a, final_b;
    unsigned int k = 0;

    while(this->differentiator.norm(x_curr_FR, x_new_FR) > this->m_tolerance){
        x_curr_FR = x_new_FR;
        Eigen::MatrixXd gradient = differentiator.computeJacobian(m_function, x_curr_FR);
        // Compute BETA. This is Fletcher-Reeves. Also compute the directions.
        double scalar_denominator = (d_old_local * d_old_local.transpose()).value(); // Extract the scalar
        double scalar_numerator = (gradient * gradient.transpose()).value(); // Extract the scalar
        BETA_Fletcher_Reeves = scalar_numerator / scalar_denominator;
        d_new_local = -gradient + BETA_Fletcher_Reeves * dir_k_local;

        // Compute the function in "s"
        std::string grad_x = "(" + std::to_string(d_new_local(0, 0)) + "*s" + to_string_with_sign(x_curr_FR["x"]) + ")";  // Gradient at x:x0 + s*dir_k 
        std::string grad_y = "(" + std::to_string(d_new_local(0, 1)) + "*s" + to_string_with_sign(x_curr_FR["y"]) + ")";  // Gradient at y:x0 + s*dir_k
        std::string substitute_function = tokenizer.replace_all(m_expression, "x", grad_x); // I want to replace variable "x" with d/dx * s
        substitute_function = tokenizer.replace_all(m_expression, "y", grad_y); // replace variable "y" with d/dy * s

        // solve for the interval in which we find a minima using Golden Section.
        std::vector<Token::TokenData> tokens = this->tokenizer.tokenize(substitute_function);
        std::queue<Token::TokenData> outputQueue = this->tokenizer.ShuntingYard(tokens);
        std::pair<double,double> result = tokenizer.golden_section(outputQueue, this->a, this->b, this->m_tolerance/100, "s");
        final_a = result.first; final_b = result.second;

        // Compute the step as the middle of found interval.
        this->step = (final_a+final_b)/2;
        // Prepare variables for next iteration
        x_new_FR["x"] = x_curr_FR["x"] + d_new_local(0,0)*this->step;
        x_new_FR["y"] = x_curr_FR["y"] + d_new_local(0,1)*this->step;
        d_old_local = gradient;
        dir_k_local = d_new_local;
        k++;
    }
    std::cout<< "The Conjugate Gradient Fletcher-Reeves found interval in "<< k <<" steps is a: "<<x_new_FR["x"]<<" and b: "<<x_new_FR["y"]<< "\n";
}

/** @brief Conjugate Gradient Solver using BETA computing by Fletcher-Reeves method */
void Conjugate_Gradient::Solver_Polak_Ribiere(){
    // initialize local variables.
    std::map<std::string, double> x_curr_PR = this->x_curr; 
    std::map<std::string, double> x_new_PR = this->x_new;
    Eigen::MatrixXd dir_k_local = this->dir_k;
    Eigen::MatrixXd d_new_local = this->d_new;
    Eigen::MatrixXd d_old_local = this->d_old;
    double final_a, final_b;
    unsigned int k = 0;

    while(this->differentiator.norm(x_curr_PR, x_new_PR) > this->m_tolerance){
        x_curr_PR = x_new_PR;
        Eigen::MatrixXd gradient = differentiator.computeJacobian(m_function, x_curr_PR);
        // Compute BETA. This is Fletcher-Reeves. Also compute the directions.
        double scalar_denominator = (d_old_local*d_old_local.transpose()).value(); // Extract the scalar.
        auto diff = gradient-d_old_local;
        double scalar_numerator =  (gradient * diff.transpose()).value(); // Extract the scalar

        BETA_Polak_Ribiere = scalar_numerator / scalar_denominator;
        d_new_local = -gradient + BETA_Polak_Ribiere * dir_k_local;

        // Compute the function in "s"
        std::string grad_x = "(" + std::to_string(d_new_local(0, 0)) + "*s" + to_string_with_sign(x_curr_PR["x"]) + ")";  // Gradient at x:x0 + s*dir_k 
        std::string grad_y = "(" + std::to_string(d_new_local(0, 1)) + "*s" + to_string_with_sign(x_curr_PR["y"]) + ")";  // Gradient at y:x0 + s*dir_k
        std::string substitute_function = tokenizer.replace_all(m_expression, "x", grad_x); // I want to replace variable "x" with d/dx * s
        substitute_function = tokenizer.replace_all(m_expression, "y", grad_y); // replace variable "y" with d/dy * s

        // solve for the interval in which we find a minima using Golden Section.
        std::vector<Token::TokenData> tokens = this->tokenizer.tokenize(substitute_function);
        std::queue<Token::TokenData> outputQueue = this->tokenizer.ShuntingYard(tokens);
        std::pair<double,double> result = tokenizer.golden_section(outputQueue, this->a, this->b, this->m_tolerance/100, "s");
        final_a = result.first; final_b = result.second;

        // Compute the step as the middle of found interval.
        this->step = (final_a+final_b)/2;
        // Prepare variables for next iteration
        x_new_PR["x"] = x_curr_PR["x"] + d_new_local(0,0)*this->step;
        x_new_PR["y"] = x_curr_PR["y"] + d_new_local(0,1)*this->step;
        d_old_local = gradient;
        dir_k_local = d_new_local;
        k++;
    }
    std::cout<< "The Conjugate Gradient Polak-Ribiere found interval in "<< k <<" steps is a: "<<x_new_PR["x"]<<" and b: "<<x_new_PR["y"]<< "\n";
}

void Conjugate_Gradient::_run(){
    int k = 0;
    std::string substitute_function = m_expression;

    // Computing the initial points and initial direction.
    dir_k = -differentiator.computeJacobian(m_function, x_new);
    std::string grad_x = "("+std::to_string(dir_k(0, 0)) +"*s" + to_string_with_sign(x_new["x"])+")";  // Gradient at x:x0 + s*dir_k 
    std::string grad_y = "("+std::to_string(dir_k(0, 1)) +"*s" + to_string_with_sign(x_new["y"])+")";  // Gradient at y:x0 + s*dir_k
    // Replace "x" with grad_x and "y" with grad_y in the function
    substitute_function = tokenizer.replace_all(m_expression, "x", grad_x); // I want to replace variable "x" with d/dx * s
    substitute_function = tokenizer.replace_all(m_expression, "y", grad_y); // replace variable "y" with d/dy * s
    // Now solve for s by equalizing d/ds(substitute_function) == 0.
    this->step = solve_for_step_2nd_order(substitute_function);
    // Now substitute to compute x_new.
    x_new["x"] = x_new["x"] + dir_k(0,0)*this->step;
    x_new["y"] = x_new["y"] + dir_k(0,1)*this->step;
    d_old = dir_k;

    // Solve using Fletcher Reeves
    this->Solver_Fletcher_Reeves();
    // Solve using Polak Ribiere
    this->Solver_Polak_Ribiere();
    
}
