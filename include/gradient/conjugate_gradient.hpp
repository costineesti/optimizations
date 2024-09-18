#ifndef CONJUGATE_GRADIENT_HPP
#define CONJUGATE_GRADIENT_HPP

#include "../syntax_tree/differentiator.hpp"
#include "../syntax_tree/ast.hpp"
#include "../tokenize/token.hpp"
#include "../Eigen/Dense"
#include <string>
#include <iostream>
#include <cmath>

class Conjugate_Gradient : public Differentiator{
    public:
        //Constructor
        Conjugate_Gradient(
            Node* function,
            std::string expression,
            std::map<std::string, double> x0,
            double tolerance
        );
        // Destructor
        ~Conjugate_Gradient();
        // run method
        virtual void _run();

    private:
        Node* m_function;
        std::map<std::string, double> x_new; // new x
        std::map<std::string, double> x_curr; // current x
        double BETA;
        Eigen::MatrixXd d_old;
        Eigen::MatrixXd d_new;
        Eigen::MatrixXd dir_k;
        double step;
        double m_tolerance;
        std::string m_expression;
        Differentiator differentiator;
        Token tokenizer;
        double solve_for_step_2nd_order(std::string& s_expression);
};

#endif
