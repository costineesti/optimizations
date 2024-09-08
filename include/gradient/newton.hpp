#ifndef NEWTON_HPP
#define NEWTON_HPP

#include "../tokenize/token.hpp"
#include "../syntax_tree/ast.hpp"
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
    
};

#endif