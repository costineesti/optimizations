#include "../../include/syntax_tree/differentiator.hpp"

Differentiator::Differentiator() {}

Node* Differentiator::differentiate(Node* root, const std::string& var) {
    if (!root) return nullptr;

    // If the node is a constant (NUMBER), derivative is 0
    if (root->data.type == Token::NUMBER) {
        return new Node(Token::TokenData(Token::NUMBER, "0"));
    }

    // If the node is a variable, return 1 if it matches the variable to differentiate with respect to
    if (root->data.type == Token::VARIABLE) {
        if (root->data.value == var) {
            return new Node(Token::TokenData(Token::NUMBER, "1"));
        } else {
            return new Node(Token::TokenData(Token::NUMBER, "0"));
        }
    }

    // If it's an operator, apply rules accordingly
    if (root->data.type == Token::OPERATOR) {
        std::string op = root->data.value;

        // Handle addition and subtraction: d(u ± v) = du ± dv
        if (op == "+" || op == "-") {
            return new Node(Token::TokenData(Token::OPERATOR, op),
                            differentiate(root->left, var),
                            differentiate(root->right, var));
        }

        // Handle multiplication: d(uv) = u * dv + v * du (Product Rule)
        if (op == "*") {
            return new Node(Token::TokenData(Token::OPERATOR, "+"),
                            new Node(Token::TokenData(Token::OPERATOR, "*"),
                                     root->left, differentiate(root->right, var)),
                            new Node(Token::TokenData(Token::OPERATOR, "*"),
                                     root->right, differentiate(root->left, var)));
        }

        // Handle division: d(u/v) = (v * du - u * dv) / v^2 (Quotient Rule)
        if (op == "/") {
            Node* left_deriv = differentiate(root->left, var);
            Node* right_deriv = differentiate(root->right, var);

            Node* numerator = new Node(Token::TokenData(Token::OPERATOR, "-"),
                                       new Node(Token::TokenData(Token::OPERATOR, "*"), root->right, left_deriv),
                                       new Node(Token::TokenData(Token::OPERATOR, "*"), root->left, right_deriv));

            Node* denominator = new Node(Token::TokenData(Token::OPERATOR, "*"), root->right, root->right);

            return new Node(Token::TokenData(Token::OPERATOR, "/"), numerator, denominator);
        }

        // Handle exponentiation (power): d(u^v) = v * u^(v-1) * du
        if (op == "^") {
            Node* exponent = root->right;
            Node* base = root->left;

            Node* new_exponent = new Node(Token::TokenData(Token::OPERATOR, "-"),
                                     exponent,
                                     new Node(Token::TokenData(Token::NUMBER, "1")));
            Node* power_deriv = new Node(Token::TokenData(Token::OPERATOR, "*"),
                                         exponent,
                                         new Node(Token::TokenData(Token::OPERATOR, "^"), base, new_exponent));
            return new Node(Token::TokenData(Token::OPERATOR, "*"), power_deriv, differentiate(base, var));
        }
    }

    // Handle functions with arguments using the chain rule
    if (root->data.type == Token::FUNCTION) {
        std::string func = root->data.value;

        // For sin(u), apply the chain rule: cos(u) * du
        if (func == "sin") {
            Node* cos_deriv = new Node(Token::TokenData(Token::FUNCTION, "cos"), root->left, nullptr);
            Node* left_deriv = differentiate(root->left, var);
            // Apply the chain rule regardless of whether the argument is an operator or a variable
            return new Node(Token::TokenData(Token::OPERATOR, "*"), cos_deriv, left_deriv);  // du, differentiate the argument
        }

        // For cos(u), apply the chain rule: -sin(u) * du
        if (func == "cos") {
            Node* sin_deriv = new Node(Token::TokenData(Token::FUNCTION, "sin"), root->left, nullptr);
            Node* left_deriv = differentiate(root->left, var);
            return new Node(Token::TokenData(Token::OPERATOR, "*"),
                            new Node(Token::TokenData(Token::OPERATOR, "-"), sin_deriv, nullptr),  // -sin(u)
                            left_deriv);  // du
        }

        // For tan(u), apply the chain rule: sec^2(u) * du
        if (func == "tan") {
            Node* left_deriv = differentiate(root->left, var);
            return new Node(Token::TokenData(Token::OPERATOR, "*"),
                            new Node(Token::TokenData(Token::FUNCTION, "sec^2"), root->left, nullptr),  // sec^2(u)
                            left_deriv);  // du
        }
    }

    return nullptr;  // Unsupported case
}


Node* Differentiator::simplify(Node* root) {
    if (!root) return nullptr;

    // Simplify subtrees first
    root->left = simplify(root->left);
    root->right = simplify(root->right);

    // If both left and right nodes are numbers, it can be evaluated
    if (root->left && root->left->data.type == Token::NUMBER && 
        root->right && root->right->data.type == Token::NUMBER) {
        
        // stack them in a queue and evaluate their operation using evaluateRPN.
        std::queue<Token::TokenData> rpnQueue;
        rpnQueue.push(root->left->data);
        rpnQueue.push(root->right->data);
        rpnQueue.push(root->data);

        // Create a Token instance to evaluate the RPN expression
        Token tokenizer;
        std::map<std::string, double> nule;
        nule["x"] = 0; nule["y"] = 0;
        double result = tokenizer.evaluateRPN(rpnQueue, nule);

        // Replace the current node with a simplified numeric result
        return new Node(Token::TokenData(Token::NUMBER, std::to_string(result)));
    }

    // Handle multiplication (*) simplification
    if (root->data.value == "*") {
        if ((root->left && root->left->data.value == "0") || 
            (root->right && root->right->data.value == "0")) {
            return new Node(Token::TokenData(Token::NUMBER, "0"));
        }
        if (root->left && (root->left->data.value == "1" || root->left->data.value == "1.000000")) {
            return root->right;
        }
        if (root->right && (root->right->data.value == "1" || root->right->data.value == "1.000000")) {
            return root->left;
        }
    }

    if (root->data.value == "^"){
        if (root->right && (root->right->data.value == "1" || root->right->data.value == "1.000000")) {
            return root->left;
        }
        if (root->right && root->right->data.value == "0") {
            return new Node(Token::TokenData(Token::NUMBER, "1"));
        }
    }

    // Handle addition (+) simplification
    if (root->data.value == "+" || root->data.value == "-") {
        if (root->left && root->left->data.value == "0") {
            return root->right;
        }
        if (root->right && root->right->data.value == "0") {
            return root->left;
        }
    }

    // Return the root if no simplification could be done
    return root;
}


std::string Differentiator::toInfix(Node* root) {
    if (!root) return "";

    // If it's a function (e.g., sin, cos), we need to wrap the argument in parentheses
    if (root->data.type == Token::FUNCTION) {
        // Ensure the left side is not null, since functions like sin, cos are unary
        if (root->left) {
            return root->data.value + "(" + toInfix(root->left) + ")";
        } else {
            return root->data.value + "()";  // Handle case where left is null (if needed)
        }
    }

    // If it's an operator, we need to handle left and right sides
    if (root->data.type == Token::OPERATOR) {
        std::string left = root->left ? toInfix(root->left) : "";
        std::string right = root->right ? toInfix(root->right) : "";

        return "(" + left + ") " + root->data.value + "(" + right + ")";
    }

    // If it's a number or a variable, just return its value
    return root->data.value;
}

/** @brief Computin the jacobian of the mathematical expression. 
 * Using Eigen because there are no matrices in C++, only an array of an array.
 */
Eigen::MatrixXd Differentiator::computeJacobian(Node* function, const std::map<std::string, double>& variablesMap){
    const int numVariables = variablesMap.size();
    Eigen::MatrixXd jacobian(1, numVariables);

    // Diferrentiate the function w.r.t every variable.
    int col = 0;
    for (const auto& [var, value] : variablesMap){
        Node* partialDerivative = this->differentiate(function, var);
        Node* simplifiedPartial = this->simplify(partialDerivative);
    }   

    return jacobian;
}

Eigen::MatrixXd Differentiator::computeHessian(Node* function, const std::map<std::string, double>& variablesMap){
    Eigen::MatrixXd hessian;
    return hessian;
}