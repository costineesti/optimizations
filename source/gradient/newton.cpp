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
        x0Vector[0] = m_x0["x"];
        x0Vector[1] = m_x0["y"];

        // Perform the update: x_new = x0 - inv(H) * G
        // This method takes us to a minima instantly. no need for tolerance check.
        Eigen::VectorXd x_newVector = x0Vector - hessianInverse * gradient.transpose(); // See if gradient needs transposed.

        // Update x_new (which is a std::map) with the new values
        std::cout<< "The Newton found interval in 1 step is a: " << x_newVector[0] << " and b: " << x_newVector[1] << "\n";
    }
    else{
        std::cerr << "Hessian is singular (non-invertible)." << "\n";
    }
}
