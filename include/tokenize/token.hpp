#indef TOKEN_HPP
#define TOKEN_HPP

#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <cmath>
#include <map>
#include <functional>
#include <math.h>
#include <stack>  // For stack
#include <queue>  // For queue

class Token {
public:

	enum class Type {
		VARIABLE,
		NUMBER,
		LEFTPARAN,
		RIGHTPARAN,
		OPERATOR,
		FUNCTION
	};

	Token();
	~Token();
	std::string value;

	void processTokens(const std::vector<Token>& tokens);
private:
	std::stack<Token> operators; // Stack for operations
	std::queue<Token> output; // Queue for the output
	std::vector<Token> tokenArray; // Array of tokens.
};

#endif