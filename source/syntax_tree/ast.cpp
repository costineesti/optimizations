#include "../../include/syntax_tree/ast.hpp"


// Template Class for a Binary Tree
BinaryTree::BinaryTree() : root(nullptr) {}


// Recursive function to delete a node from the tree in postorder
Node* BinaryTree::deleteRecursive(Node* current, Token::TokenData value) {
	if (current == nullptr) return nullptr;

	// First, recursively delete in left and right subtrees
	current->left = deleteRecursive(current->left, value);
	current->right = deleteRecursive(current->right, value);

	// Now process the current node
	if (current->data == value) {
		// Case 1: Node with no children (leaf node)
		if (current->left == nullptr && current->right == nullptr) {
			delete current;
			return nullptr;
		}
		// Case 2: Node with only a right child
		if (current->left == nullptr) {
			Node* temp = current->right;
			delete current;
			return temp;
		}
		// Case 3: Node with only a left child
		if (current->right == nullptr) {
			Node* temp = current->left;
			delete current;
			return temp;
		}
		// Case 4: Node with two children
		Node* successor = findMin(current->right);
		current->data = successor->data;
		current->right = deleteRecursive(current->right, successor->data); // Correct the recursive call here
	}

	return current; // Return the current node, possibly modified
}

// Helper Function to find the last left value node
Node* BinaryTree::findMin(Node* node) {
	while (node->left != nullptr) node = node->left;
	return node;
}

// Recursive Function to search for a value in the tree
bool BinaryTree::searchRecursive(Node* current, Token::TokenData value) {
	if (current == nullptr) return false;
	if (current->data == value) return true;
	return searchRecursive(current->left, value) || searchRecursive(current->right, value);
}

// Function for Recursive postorder traversal of the tree (as RPN is POSTORDER)
std::string BinaryTree::postorderRecursive(Node* node) {
	std::string result;
	if (node != nullptr) {
		result += postorderRecursive(node->left);
		result += postorderRecursive(node->right);
		result += node->data.value + " ";
	}
	return result;
}

// function to insert a node in the binary tree
void BinaryTree::insertNode(Token::TokenData value) {
	Node* newNode = new Node(value);

	if (root == nullptr) {
		root = newNode;
		return;
	}

	std::queue<Node*> q; // queue of pointers to nodes in a tree
	q.push(root);

	while (!q.empty()) {
		Node* current = q.front();
		q.pop();

		if (current->left == nullptr) {
			current->left == newNode;
			return;
		}
		else {
			q.push(current->left);
		}

		if (current->right == nullptr) {
			current->right = newNode;
			return;
		}
		else {
			q.push(current->right);
		}
	}
}

// Function to delete a node from the tree
void BinaryTree::deleteNode(Token::TokenData value) {
	root = deleteRecursive(root, value);
}
// Function to search for a value in the tree
bool BinaryTree::search(Token::TokenData value) {
	return searchRecursive(root, value);
}
// Function  to perform level order traversal of the tree
void BinaryTree::levelOrder() {
	if (root == nullptr) return;

	std::queue<Node*> q;
	q.push(root);

	while (!q.empty()) {
		Node* current = q.front();
		q.pop();

		std::cout << current->data << " ";

		if (current->left != nullptr) q.push(current->left);
		if (current->right != nullptr) q.push(current->right);
	}
	std::cout << std::endl;
	}

AST::AST() {}

// Function to perform postorder traversal of the tree
std::string AST::postorder(Node* root) {
	return postorderRecursive(root);
}
// Function to build the Abstract Syntax Tree
Node* AST::buildAST(const std::queue<Token::TokenData>& rpnQueue) {
	std::stack<Node*> stack; // stack of pointers to binary tree node.
	std::queue<Token::TokenData> queue = rpnQueue; // Copy the queue to preserve the original.

	while (!queue.empty()) {
		Token::TokenData token = queue.front();
		queue.pop();

		Node* newNode = new Node(token); // a new node based on the current token

		if (token.type == Token::NUMBER || token.type == Token::VARIABLE) {
			stack.push(newNode); // Push operands directly onto the stack
		}
		else if (token.type == Token::OPERATOR) {
			Node* right = stack.top(); // Pop nodes for the operator's operands from the stack
			stack.pop();

			Node* left = nullptr;
			if (!stack.empty()) {
				left = stack.top();
				stack.pop();
			}

			// Create a new node with the operator/function and set the operands as it's children
			newNode->right = right;
			newNode->left = left;

			// Push the new subtree back onto the stack
			stack.push(newNode);
		}
		else if (token.type == Token::FUNCTION) {
			// Pop only one operand for unary functions
			Node* operand = stack.top();
			stack.pop();

			// Create a new node with the function and set the operand as its child
			newNode->left = operand;

			// Push the new subtree back onto the stack
			stack.push(newNode);
		}

	}

	// The remaining node on the stack is the root of the AST
	this->root = stack.top();
	return stack.top();
}