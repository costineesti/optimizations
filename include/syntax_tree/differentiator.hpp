#ifndef DIFFERENTIATOR_HPP
#define DIFFERENTATOR_HPP
#include "./ast.hpp"

class Differentiator : public AST{
    public:
        Differentiator();
        // Function that returns the derivative inside a tree.
        Node* differentiate(Node* root, const std::string& var);
        // Function that converts the AST to the infix notation ( as i give it )
         std::string toInfix(Node* root);
         // Function that simplifies an AST that has multiple 0 in it (such as multiply etc)
         Node* simplify(Node *root);
};

#endif