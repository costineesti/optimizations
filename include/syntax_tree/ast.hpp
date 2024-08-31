#ifndef AST_HPP
#define AST_HPP

#include <iostream>
#include <queue>
#include <stack>
#include "../tokenize/token.hpp"

// Node that takes a token as value
class Node {
public:
	Token::TokenData data;
	Node* left;
	Node* right;

	Node(Token::TokenData value) : data(value), left(nullptr), right(nullptr) {}
};

// Binary Tree
class BinaryTree {
protected: // in order to be directly accessible in the derived class, AST.
	Node* root;

	Node* deleteRecursive(Node* current, Token::TokenData value);
	Node* findMin(Node* node);
	bool searchRecursive(Node* current, Token::TokenData value);
	std::string postorderRecursive(Node* node);

public:
	BinaryTree();

	void insertNode(Token::TokenData value);
	void deleteNode(Token::TokenData value);
	bool search(Token::TokenData value);
	std::string postorder();
	void levelOrder();
};

class AST : public BinaryTree {
public:
	AST();
	Node* buildAST(const std::queue<Token::TokenData>& rpnQueue);
	std::string postorder();
	Node* getRoot() const {
		return root;
	}
};

#endif