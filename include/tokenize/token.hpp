#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <cmath>
#include <map>
#include <functional>
#include <math.h>
#include <stack>  // For operations
#include <queue>  // For queue
#include <array> //  For tokens.

class Token {
public:

	/* Constructor */
	Token();
	/* Desctructor */
	~Token();

	enum TokenType { NUMBER, VARIABLE, OPERATOR, FUNCTION, LEFT_PARAN, RIGHT_PARAN };
	struct TokenData {
		TokenType type;
		std::string value;
		TokenData(TokenType t, const std::string& expr) : type(t), value(expr) {}
	};

	std::string tokenTypeToString(TokenData tkData);
	std::vector<Token::TokenData> tokenize(const std::string &expr);
	std::queue<Token::TokenData> ShuntingYard(const std::vector<Token::TokenData> &tokens);
	double evaluateRPN(std::queue<Token::TokenData> &outputQueue, double xValue, double yValue);

private:
	int getPrecedence(const std::string &op);
	bool isLeftAssociative(const std::string &op);
};

#endif