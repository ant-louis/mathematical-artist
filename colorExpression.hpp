#ifndef GUARD_colorExpression_h
#define GUARD_colorExpression_h

#include "parser.hpp"//to use typedef Exp

#include <iostream>
#include <string>

class ColorExpression
{

public :

	void new_exp(int depth); //Initialise a new random color expression

	void new_exp(std::istream& in); //Initialise a new color expression if the user wants to write its own expressions

	int calculate_depth(); //Calculates the depth of the expression the user wrote

	double compute_value(double x, double y); //Returns the result of the color expression for a given point (x,y)

	std::string rpn_to_infix() const; //Returns a string corresponding to the color expression in infix notation

	
private :

	Exp rpn_exp;

	void make_random_rpn_exp(int depth); //Create the rpn_exp vector corresponding to an expression of a given depth

};

#endif