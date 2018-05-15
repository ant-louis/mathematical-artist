#ifndef GUARD_parser_h
#define GUARD_parser_h

#include <iostream>
#include <string>
#include <ios> //streamoff
#include<vector>


/*********************************************************LEXER****************************************************************************
*****************************************************************************************************************************************/
class Lexer
{

public:

	enum token {X, Y, SIN, COS, PI, OPEN_PAR, CLOSE_PAR, TIMES, AVG, COMMA};

	explicit Lexer(std::istream& in); //Constructor

	token next(); //returns the next token, removing it from the input
	token peek(); //returns the next token, but without removing it from the input
	std::streamoff count() const; //returns the number of characters that have been read from the istream since the last call to reset()
	void reset(); //resets the character count to zero

private:
	
	std::istream& input;
	std::streamoff counter;

	token analyze(const char& c, bool extract_char); //analyze an extracted char from the input

};



/**************************************************************PARSER*******************************************************************
***************************************************************************************************************************************/

typedef std::vector<std::string> Exp;


class Parser
{
public:

	explicit Parser(std::istream& in); //Constructor
	bool parse(Exp& exp); //returns true if it success-fully parsed a complete expression, and false if the expression is incomplete

private :
	
	Lexer lexer;
	std::vector<Lexer::token> token_vec; //vector containing the tokens read from the lexer
	size_t nb_par; //variable used to check if there are as many close par as open par

	bool check_syntax(); //check the syntax of an expression
	bool check_sin_cos(); //check the syntax of the sin/cos expression
	bool check_product(); //check the syntax of the product expression
	bool check_avg(); //check the syntax of the average expression

	bool check_after_token(); //check if there is a valid token after one
	int check_size_token(); //check size of the next token
	
	void infix_to_rpn(Exp& exp); //convert the infix sequence of tokens in a RPN keystrokes' sequence

	std::string token_to_string(const Lexer::token& tok); //convert a token to a string
	bool greater_precedence(const Lexer::token& tok, const std::string& str); //check the precedence between tokens

};


#endif