//#inlcude <main.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <cmath>
#include <map>
#include <functional>
#include <math.h>


int main() {
	std::string expression;

	std::cout << "Enter a mathematical expression in terms of x: ";
	std::getline(std::cin, expression);

	double x;
	std::cout << "Enter the value of x: ";
	std::cin >> x;

	try {
		double result = parseAndEvaluate(expression, x);
		std::cout << "For x = " << x << ", the expression '" << expression << "' evaluates to " << result << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}

	return 0;
}


