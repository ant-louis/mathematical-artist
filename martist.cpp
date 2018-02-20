#include "martist.hpp"
#include "colorExpression.hpp"

#include <iostream>//std::istream, std::ostream
#include <sstream>//std::istringstream
#include <string>//std::string, std::getline
#include <stdexcept>//domain_error
#include <vector>//std::vector
#include <cstdlib>// std::rand, std::srand
#include <iterator>//begin(), end()
#include <algorithm>//std::all_of
#include <cctype>//std::isspace
#include <cstddef>//nullptr
#include <cstdio> // eof()


using std::string;
using std::vector;
using std::istream;
using std::ostream;



/********************************************************************************************************************************
* Constructor
*
* ARGUMENTS :
*	- buffer is a "passed-in" array to be the buffer containing the image pixels 
*	- width and height are the respective dimensions of the image (in pixels)
*	- rdepth, gdepth, bdepth are the depths for expressions representing the red, green and blue colour components, respectively
**********************************************************************************************************************************/
Martist::Martist(unsigned char* buffer, size_t width, size_t height, int rdepth, int gdepth, int bdepth) : 
	my_buffer(buffer),
	my_width(width),
	my_height(height),
	rdepth(rdepth), 
	gdepth(gdepth), 
	bdepth(bdepth)
{
	if(buffer == nullptr)
		throw std::domain_error("ERROR : Buffer is empty.");

	if((int)width <= 0 || (int)height <= 0)
		throw std::domain_error("ERROR : Width or height can't be negative.");

	if(rdepth < 0 || gdepth < 0 || bdepth < 0)
		throw std::domain_error("ERROR : Depth of expressions can't be negative.");
}




/******************************************************************************************************************************
* Set the depth of the red expression 
*
* ARGUMENT :
*	- depth is the depth of the red expression
*******************************************************************************************************************************/
void Martist::redDepth(int depth){
	if(depth < 0)
		throw std::domain_error("ERROR : Depth of expressions can't be negative.");
	rdepth = depth;
}


/******************************************************************************************************************************
* Get the depth of the red expression 
*
* ARGUMENT : /
*******************************************************************************************************************************/
int Martist::redDepth() const{
	return rdepth;
}


/******************************************************************************************************************************
* Set the depth of the green expression 
*
* ARGUMENT :
*	- depth is the depth of the green expression
*******************************************************************************************************************************/
void Martist::greenDepth(int depth){
	if(depth < 0)
		throw std::domain_error("ERROR : Depth of expressions can't be negative.");
	gdepth = depth;
}


/******************************************************************************************************************************
* Get the depth of the green expression 
*
* ARGUMENT : /
*******************************************************************************************************************************/
int Martist::greenDepth() const{
	return gdepth;
}


/******************************************************************************************************************************
* Set the depth of the blue expression 
*
* ARGUMENT :
*	- depth is the depth of the blue expression
*******************************************************************************************************************************/
void Martist::blueDepth(int depth){
	if(depth < 0)
		throw std::domain_error("ERROR : Depth of expressions can't be negative.");
	bdepth = depth;
}


/******************************************************************************************************************************
* Get the depth of the blue expression 
*
* ARGUMENT : /
*******************************************************************************************************************************/
int Martist::blueDepth() const{
	return bdepth;
}


/******************************************************************************************************************************
* Seed the randomness
*
* ARGUMENT : 
*	- seed is 
*******************************************************************************************************************************/
void Martist::seed(int seed){
	std::srand(seed);
}



/********************************************************************************************************************************
* Change the image buffer
*
* ARGUMENTS :
*	- buffer is a "passed-in" array to be the buffer containing the image pixels 
*	- width and height are the respective dimensions of the image (in pixels)
**********************************************************************************************************************************/
void Martist::changeBuffer(unsigned char* buffer, size_t width, size_t height){

	if(buffer == nullptr)
		throw std::domain_error("ERROR : Buffer is empty.");

	if((int)width <= 0 || (int)height <= 0)
		throw std::domain_error("ERROR : Width or height can't be negative.");

	my_buffer = buffer;
	my_width = width;
	my_height = height;
}




/******************************************************************************************************************************
* Generate a new random image 
*
* ARGUMENT : /
*******************************************************************************************************************************/
void Martist::paint(){

	red_exp.new_exp(rdepth);
	green_exp.new_exp(gdepth);
	blue_exp.new_exp(bdepth);

	compute_buffer();
}


/******************************************************************************************************************************
* Compute the buffer with the different color expressions
*
* ARGUMENT : /
*******************************************************************************************************************************/
void Martist::compute_buffer(){

	for(float j = 0; j < my_height; j++){
		for(float i = 0; i < my_width; i++){

			size_t k = (i + j*my_width)*3;
			double exp_value;

			double x = (2*i)/(my_width-1) - 1;
			double y = (2*j)/(my_height-1) - 1;

			// RED
			exp_value = red_exp.compute_value(x, y);
			my_buffer[k] = simple_scaling(exp_value);

			// GREEN
			exp_value = green_exp.compute_value(x, y);
			my_buffer[k+1] = simple_scaling(exp_value);

			// BLUE
			exp_value = blue_exp.compute_value(x, y);
			my_buffer[k+2] = simple_scaling(exp_value);

		}
	}
}

/***************************************************************************************************************
* Returns an unsigned char [0,255] corresponding to the scaling of the given double value
* 
* ARGUMENTS : 
*	- value is the value to scale
****************************************************************************************************************/
unsigned char Martist::simple_scaling(double value){
	unsigned char result;

	if(value == 0.0)
		result = 0;
	else
		result = 255/2 * (value + 1);

	return result;
}


/********************************************************************************************************************************
* Overloading output operator
*
* ARGUMENTS :
*	- out is the output stream
*	- m is an object of the class Martist
**********************************************************************************************************************************/
ostream& operator<<(ostream &out, const Martist& m){
    
    out << "red = " << m.red_exp.rpn_to_infix() << std::endl;

    out << "green = " << m.green_exp.rpn_to_infix() << std::endl;

    out << "blue = " << m.blue_exp.rpn_to_infix() << std::endl;
 
    return out;
}
 


/********************************************************************************************************************************
* Overloading input operator
*
* ARGUMENTS :
*	- in is the input stream
*	- m is an object of the class Martist
**********************************************************************************************************************************/
istream& operator>>(istream& in, Martist& m){

	string red, green, blue;

	if(in){

		// The first line to read is RED : user has to write "red= exp"
		//Check the whitespaces before string "red"
		while(isspace(in.peek())){
			in.get();
			if(in.eof() || in.peek() == '\n')
				throw std::domain_error("ERROR : bad argument entered.");
			continue;
		}
		//Check if we have "red="
		if(in.get() != 'r')
			throw std::domain_error("ERROR : bad argument entered.");
		if(in.get() != 'e')
			throw std::domain_error("ERROR : bad argument entered.");
		if(in.get() != 'd')
			throw std::domain_error("ERROR : bad argument entered.");
		if(in.get() != '=')
			throw std::domain_error("ERROR : bad argument entered.");
		//Get all the expressions after the '=' till the '\n'
		std::getline(in, red); 
		//If the string is full of whitespaces only or empty, make an expression of depth 0
		if(std::all_of(red.begin(), red.end(), isspace) || red.empty()){
			m.red_exp.new_exp(0);
		}
		else{
			//Otherwise, make an expression with the input string
			std::istringstream is_red(red);
			m.red_exp.new_exp(is_red);
		}



		//The second line to read is GREEN : user has to write "green= exp"
		//Check the whitespaces before string "green"
		while(isspace(in.peek())){
			in.get();
			if(in.eof() || in.peek() == '\n')
				throw std::domain_error("ERROR : bad argument entered.");
			continue;
		}
		//Check we have "green="
		if(in.get() != 'g')
			throw std::domain_error("ERROR : bad argument entered.");
		if(in.get() != 'r')
			throw std::domain_error("ERROR : bad argument entered.");
		if(in.get() != 'e')
			throw std::domain_error("ERROR : bad argument entered.");
		if(in.get() != 'e')
			throw std::domain_error("ERROR : bad argument entered.");
		if(in.get() != 'n')
			throw std::domain_error("ERROR : bad argument entered.");
		if(in.get() != '=')
			throw std::domain_error("ERROR : bad argument entered.");
		//Get all the expressions after the '=' till the '\n'
		std::getline(in, green);
		//If the string is full of whitespaces only or empty, make an expression of depth 0
		if(std::all_of(green.begin(), green.end(), isspace) || green.empty()){
			m.green_exp.new_exp(0);
		}
		else{
			//Otherwise, make an expression with the input string
			std::istringstream is_green(green);
			m.green_exp.new_exp(is_green);
		}



		//The third and last line to read is BLUE : user has to write "blue= exp"
		//Check the whitespaces before string "blue"
		while(isspace(in.peek())){
			in.get();
			if(in.eof() || in.peek() == '\n')
				throw std::domain_error("ERROR : bad argument entered.");
			continue;
		}
		//Check we have "blue="
		if(in.get() != 'b')
			throw std::domain_error("ERROR : bad argument entered.");
		if(in.get() != 'l')
			throw std::domain_error("ERROR : bad argument entered.");
		if(in.get() != 'u')
			throw std::domain_error("ERROR : bad argument entered.");
		if(in.get() != 'e')
			throw std::domain_error("ERROR : bad argument entered.");
		if(in.get() != '=')
			throw std::domain_error("ERROR : bad argument entered.");
		//Get all the expressions after the '=' till the '\n'
		std::getline(in, blue);
		//If the string is full of whitespaces only or empty, make an expression of depth 0
		if(std::all_of(blue.begin(),blue.end(),isspace) || blue.empty()){
			m.blue_exp.new_exp(0);
		}
		else{
			//Otherwise, make an expression with the input string
			std::istringstream is_blue(blue);
			m.blue_exp.new_exp(is_blue);
		}

		//Calculate the depths of the new expressions
		m.rdepth = m.red_exp.calculate_depth();
		m.gdepth = m.green_exp.calculate_depth();
		m.bdepth = m.blue_exp.calculate_depth();

		//Update the buffer
		m.compute_buffer();

		//Clear the input stream
		in.clear();
	}

    return in;
}