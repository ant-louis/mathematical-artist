#include "colorExpression.hpp"
#include "parser.hpp"

#include <math.h> //M_PI
#include <string>
#include <vector>
#include <iostream>
#include <cstdlib> // std::rand


using std::string;
using std::vector;


/********************************************************************************************************************************
* Initialise a new random color expression
*
* ARGUMENTS :
*	- depth is the depth of the random expression
**********************************************************************************************************************************/
void ColorExpression::new_exp(int depth){

	//Get rid of previous content
	rpn_exp.clear();

	//Create rpn_exp
	make_random_rpn_exp(depth); 
}




/********************************************************************************************************************************
* Initialise a new color expression if the user wants to write its own expressions
*
* ARGUMENTS :
*	- in is the input of the user
**********************************************************************************************************************************/
void ColorExpression::new_exp(std::istream& in){

	//Get rid of previous content
	rpn_exp.clear();

	Parser parser(in);

	try{
		//Create rpn_exp
		if(!parser.parse(rpn_exp))
			std::cout << "Parse failed, incomplete expression." << std::endl;
	}catch(std::domain_error& e){
		throw;
	}
}




/********************************************************************************************************************************
* Create the rpn_exp vector corresponding to an expression of a given depth
*
* ARGUMENTS :
*	- depth is the depth of the expression we want to create
**********************************************************************************************************************************/
void ColorExpression::make_random_rpn_exp(int depth){

	string basic_exp[2] = {"x", "y"};
	string composed_exp[4] = {"sin","cos","avg","*"};
	int random_choice, random_nummer, random_depth, depth_exp1, depth_exp2;
	
	if(depth > 1){

		random_choice = std::rand()%4;
		string exp = composed_exp[random_choice];

		// If it's a sin or a cos
		if(random_choice == 0 || random_choice == 1){
			//Append pi
			rpn_exp.push_back("pi");
			// Append a random expression of depht "depth-1"
			make_random_rpn_exp(depth-1);
			//Append pi
			rpn_exp.push_back("*");
			//Append sin/cos
			rpn_exp.push_back(exp);
		}

		// If it's an average : one of the exp is of depth "depth-1" and the other has a depth in [1, depth-1]
		else if(random_choice == 2){

			//Get a random depth between [1, depth-1] for one of the expression in average
			random_depth = std::rand()%(depth-1) + 1;

			// Choose which is expression is going to be of random_depth
			random_nummer = std::rand()%2;

			if(random_nummer == 0){
				depth_exp1 = random_depth;
				depth_exp2 = depth-1;
			}else{
				depth_exp1 = depth-1;
				depth_exp2 = random_depth;
			}

			// Append a random expression of the corresponding depth
			make_random_rpn_exp(depth_exp1);
			//Append another expression of the corresponding depth
			make_random_rpn_exp(depth_exp2);
			//Append avg
			rpn_exp.push_back(exp);
		}

		// If it's a product
		else if(random_choice == 3){
			//Get a random depth between [1, depth-1] for one of the expression in average
			random_depth = std::rand()%(depth-1) + 1;

			// Choose which is expression is going to be of random_depth
			random_nummer = std::rand()%2;

			if(random_nummer == 0){
				depth_exp1 = random_depth;
				depth_exp2 = depth-1;
			}else{
				depth_exp1 = depth-1;
				depth_exp2 = random_depth;
			}
			// Append a random expression of the corresponding depth
			make_random_rpn_exp(depth_exp1);
			//Append another expression of the corresponding depth
			make_random_rpn_exp(depth_exp2);
			//Append a times
			rpn_exp.push_back(exp);

		}
	}

	else if(depth == 1){
		// Get a random basic expression of depth 1
		random_choice = std::rand()%2;
		rpn_exp.push_back(basic_exp[random_choice]);
	}

	else{
		// If depth == 0
		rpn_exp.push_back("0");
	}

}




/***************************************************************************************************************
* Returns the result of the color expression for a given point (x,y)
*
* ARGUMENTS : 
*	- x and y the position in the table of pixels
****************************************************************************************************************/
double ColorExpression::compute_value(double x, double y){

	vector<double> numbers;

	// If expression is of depth 0
	if(rpn_exp.back() == "0")
		return 0.0;

	for(vector<string>::size_type i=0; i != rpn_exp.size(); i++){

		string tok = rpn_exp[i];

		if(tok == "x" || tok == "y" || tok == "pi"){

			if(tok == "x"){
				numbers.push_back(x);
			}
			else if(tok == "y"){
				numbers.push_back(y);
			}
			else if(tok == "pi"){
				numbers.push_back(M_PI);
			}
		}

		else if(tok == "sin" || tok == "cos"){

			double op = numbers.back();
			numbers.pop_back();

			if(tok == "sin"){
				numbers.push_back(sin(op));
			}
			else if(tok == "cos"){
				numbers.push_back(cos(op));
			}
		}
		else{

			double op2 = numbers.back();
			numbers.pop_back();
			double op1 = numbers.back();
			numbers.pop_back();

			if(tok == "avg"){
				numbers.push_back((op1+op2)/2);
			}
			else if(tok == "*"){
				numbers.push_back(op1 * op2);
			}
		}

	}

	return numbers.back();
}




/******************************************************************************************************************************
* Calculates the depth of the expression the user wrote
*
* ARGUMENT : /
*******************************************************************************************************************************/
int ColorExpression::calculate_depth(){
	int current_depth = 1;
	int max_depth = 1;

	string infix_exp = rpn_to_infix();
	int size = infix_exp.length();

	if(infix_exp.empty())
		return 0;

	for(int i = 0; i < size; i++){

		if(infix_exp[i] == '('){

			current_depth++;

			if(current_depth > max_depth)
				max_depth = current_depth;
		}

		else if(infix_exp[i] == ')'){

			if(current_depth > 1)
				current_depth--;
		}
	}

	return max_depth;
}




/******************************************************************************************************************************
* Returns a string corresponding to the color expression in infix notation
*
* ARGUMENT : /
*******************************************************************************************************************************/
string ColorExpression::rpn_to_infix() const{

	vector<string> infix_exp;

	// If expression is of depth 0
	if(rpn_exp.back() == "0")
		return rpn_exp.back();

	for(vector<string>::size_type i=0; i != rpn_exp.size(); i++){

		string tok = rpn_exp[i];

		if(tok == "x" || tok == "y" || tok == "pi")
				infix_exp.push_back(tok);

		else if(tok == "sin" || tok == "cos"){
			//Pop the operand of sin/cos
			string op = infix_exp.back();
			infix_exp.pop_back();

			//Append it to sin/cos
			tok.append(op);
			//Push the expression back onto the stack
			infix_exp.push_back(tok);

		}
		else{
			//Pop the two last elements
			string op2 = infix_exp.back();
			infix_exp.pop_back();
			string op1 = infix_exp.back();
			infix_exp.pop_back();

			if(tok == "avg"){
				tok.append("(");
				tok.append(op1);
				tok.append(",");
				tok.append(op2);
				tok.append(")");
				infix_exp.push_back(tok);
			}
			else if(tok == "*"){
				string exp;
				exp.append("(");
				exp.append(op1);
				exp.append(tok);
				exp.append(op2);
				exp.append(")");
				infix_exp.push_back(exp);
			}
		}

	}
	
	return infix_exp.back();
}