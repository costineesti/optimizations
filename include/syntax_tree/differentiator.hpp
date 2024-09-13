#ifndef DIFFERENTIATOR_HPP
#define DIFFERENTIATOR_HPP

#include "./ast.hpp"
#include "../Eigen/Dense"

class Differentiator : public AST{
    public:
        Differentiator();
        /** @brief Function that returns the derivative of a function inside a tree. */
        Node* differentiate(Node* root, const std::string& var);
        /** @brief Function that converts the AST to the infix notation */
        std::string toInfix(Node* root);
        /** @brief Function that simplifies an AST */
        Node* simplify(Node *root);
        /** @brief compute the jacobian using Eigen */
        Eigen::MatrixXd computeJacobian(Node* function, const std::map<std::string, double>& variablesMap);
        /** @brief compute the hessian using Eigen */
        Eigen::MatrixXd computeHessian(Node* function, const std::map<std::string, double>& variablesMap);
        /** @brief Compute the norm between two vectors */
        double norm(const std::map<std::string, double>& point1, const std::map<std::string, double>& point2);
};

#endif