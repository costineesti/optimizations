# optimizations

Sources: [AST](https://costinchitic.co/notes/Abstract-Syntax-Tree), [Shunting Yard](https://costinchitic.co/notes/Shunting-yard-algorithm)

## Optimization algorithms in C++:
* Newton
* Conjugate Gradient
* Steepest Descent

The main idea is to tokenize an input string that contains multiple variables (x,y,z, etc) and to solve for the solutions.
I solved for a local minima using the algorithms above. The tokenized input is transformed into an AST.
Then I compute the differentials of that Abstract Syntax Tree (which is basically just a binary tree) and compute the values.
The values of the input string are computed through Shunting Yard algorithm and Reverse Polish Notation (RPN).
