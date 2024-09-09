#ifndef NEWTON_HPP
#define NEWTON_HPP

#include "../syntax_tree/differentiator.hpp"
#include "../Eigen/Dense"
#include <string>
#include <iostream>
#include <cmath>

/** @brief
This class is only a test for the moment, where I want to see if 
by using Eigen I can store the jacobian and the hessian of a multivariate function
 */
class Newton : public Differentiator {
    public:
        // Constructor
        Newton(
            Node* function,
            std::map<std::string, double> x0
        );
        // Destructor
        ~Newton();
        virtual void _run();
    private:
        Node* m_function;
        std::map<std::string, double> m_x0;
        Differentiator diff;
        std::map<std::string, double> x_new;
}; // class Newton

#endif // Newton.hpp