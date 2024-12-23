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

/** @brief: This class is supposed to tokenize a mathematical expression 
After tokenizing, it will be passed further to the Shunting Yard algorithm or the AST.
*/
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

		// define equality operator
		bool operator==(const TokenData& other) const {
			return (type == other.type && value == other.value);
		}

		// Define stream output operator "<<"
		friend std::ostream& operator<<(std::ostream& os, const TokenData& token){
			os << token.value;
			return os;
		}
	};

	std::string replace_all(std::string& str, const std::string& from, const std::string& to);
	std::string tokenTypeToString(TokenData tkData);
	std::vector<Token::TokenData> tokenize(const std::string &expr);
	std::queue<Token::TokenData> ShuntingYard(const std::vector<Token::TokenData> &tokens);
	double evaluateRPN(std::queue<Token::TokenData> outputQueue, const std::map<std::string, double>& variableValues);
	std::pair<double,double> golden_section(std::queue<Token::TokenData> outputQueue, double a, double b, double e, const char* variableName);
	std::pair<double,double> fibonacci_series(std::queue<Token::TokenData> outputQueue, double a, double b, double e, const char* variableName);

private:
	int getPrecedence(const std::string &op);
	bool isLeftAssociative(const std::string &op);
};

#endif