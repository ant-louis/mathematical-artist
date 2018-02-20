#ifndef GUARD_martist_h
#define GUARD_martist_h

#include "colorExpression.hpp"

#include <string>
#include <iostream>


class Martist
{

public:

	explicit Martist(unsigned char* buffer, size_t width, size_t height, int rdepth, int gdepth, int bdepth); // Constructor

	void redDepth(int depth); // Set the depth of the red expression 

	int redDepth() const; // Get the depth of the red expression 

	void greenDepth(int depth); // Set the depth of the green expression 

	int greenDepth() const; // Get the depth of the green expression 

	void blueDepth(int depth); //  Set the depth of the blue expression

	int blueDepth() const; // Get the depth of the blue expression 

	void seed(int seed); // Seed the randomness

	void changeBuffer(unsigned char* buffer, size_t width, size_t height); // Change the image buffer

	void paint(); // Generate a new random image 

	friend std::ostream& operator<< (std::ostream& out, const Martist& m); // Overloading output operator

	friend std::istream& operator>> (std::istream& in, Martist& m); // Overloading input operator

	
private:

	unsigned char* my_buffer;

	size_t my_width;
	size_t my_height;

	int rdepth;
	int gdepth;
	int bdepth;

	ColorExpression red_exp;
	ColorExpression green_exp;
	ColorExpression blue_exp;

	void compute_buffer(); //Compute the buffer with the different color expressions
	unsigned char simple_scaling(double value); //Returns an unsigned char [0,255] corresponding to the scaling of the given double value
	
};

#endif