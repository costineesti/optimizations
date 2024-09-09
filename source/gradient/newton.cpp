#include "../../include/gradient/newton.hpp"

/** @brief Class constructor
 * @param function: as a Node*,
 * @param tolerance as a double.
 */
Newton::Newton(
    Node* function,
    std::map<std::string, double> x0
)
: m_function(function)
, m_x0(x0)
, x_new{{"x", 0.0}, {"y", 0.0}}
{}

/** @brief Class Destructor */
Newton::~Newton(){}

void Newton::_run(){

    auto gradient = this->diff.computeJacobian(m_function, m_x0);
    auto hessian = this->diff.computeHessian(m_function, m_x0);

    if (hessian.determinant() != 0){
        Eigen::MatrixXd hessianInverse = hessian.inverse();

        // Convert m_x0 (std::map) to an Eigen vector
        Eigen::VectorXd x0Vector(m_x0.size());
        int i = 0;
        for (const auto& [var, value] : m_x0) {
            x0Vector(i) = value; // Fill Eigen vector with values from m_x0
            i++;
        }

        // Perform the update: x_new = x0 - inv(H) * G
        // This method takes us to a minima instantly. no need for tolerance check.
        Eigen::VectorXd x_newVector = x0Vector - hessianInverse * gradient.transpose(); // See if gradient needs transposed.

        // Update x_new (which is a std::map) with the new values
        i = 0;
        for (auto& [var, value] : x_new) {
            value = x_newVector(i);  // Update the map with the new values
            std::cout << var << ":" << value << " ";
            i++;
        }
        std::cout << std::endl;
    }
    else{
        std::cerr << "Hessian is singular (non-invertible)." << std::endl;
    }
}