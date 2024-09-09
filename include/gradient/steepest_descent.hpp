#ifndef STEEPEST_DESCENT_HPP
#define STEEPEST_DESCENT_HPP

#include "../syntax_tree/differentiator.hpp"
#include "../tokenize/token.hpp"
#include "../Eigen/Dense"
#include <string>
#include <iostream>
#include <cmath>

/** @brief
 * This class implements the optimization algorithm based on Gradient : Steepest Descent.
 * Based on a variable step, it finds the minimum of the given function.
 */
class Steepest_Descent : public Differentiator
{
    public:
        // Constructor
        Steepest_Descent(
            Node* function,
            std::string expression,
            double tolerance,
            double a,
            double b,
            std::map<std::string, double> x0
        );
        // Destructor
        ~Steepest_Descent();
        // run method
        virtual void _run();
    
    private:
        Node* m_function;
        std::string m_expression;
        double m_tolerance;
        double d;
        double a;
        double b;
        double step;
        std::map<std::string, double> x_new; // x_new
        std::map<std::string, double> x_curr; // current x
        Differentiator differentiator;
        Token tokenizer;
};

#endif