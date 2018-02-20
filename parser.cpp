#include "parser.hpp"

#include <iostream>
#include <string>
#include <stdexcept> //domain_error
#include <cstdio> // eof()
#include <ios> // streamoff
#include <cctype> //isspace
#include <vector>

using std::string;
using std::vector;



/*********************************LEXER_CONSTRUCTOR**************************************
*
*****************************************************************************************/
Lexer::Lexer(std::istream& in): input(in), counter(0){}


/*******************************************next**************************************************
*
* ARGUMENTS : /
*
* RETURN : the next token of the input stream (deleting the corresponding input sequence)
*
***************************************************************************************************/
Lexer::token Lexer::next(){

	Lexer::token tok;
	char first;
	
	first = input.get();
	counter++;
	//Get the first character and check if input is not empty
	if(input.eof() || input.peek() == '\n'){
		throw std::domain_error("EOI");
	}
	

	//Check if the first character is a whitespace -> get the next one if it is
	while(isspace(first)){
		first = input.get();
		counter++;
		if(input.eof() || input.peek() == '\n'){
			throw std::domain_error("EOI");
		}
		
	}

	//Analyze of the input
	tok = analyze(first, true);

	return tok;
}


/*******************************************peek**************************************************
*
* ARGUMENTS : /
*
* RETURN : the next token of the input stream (without delating the corresponding input sequence)
*
***************************************************************************************************/
Lexer::token Lexer::peek(){

	Lexer::token tok;
	char first;
	int nb_whites = 0;

	first = input.get();
	//Get the first character and check if input is not empty
	if(input.eof() || input.peek() == '\n'){
		throw std::domain_error("EOI");
	}
	

	//Check if the first character is a whitespace -> get the next one if it is
	while(isspace(first)){
		first = input.get();
		nb_whites++;
		if(input.eof() || input.peek() == '\n'){
			throw std::domain_error("EOI");
		}
		
	}

	//Analyze of the input
	tok = analyze(first, false);

	// Put back the first char of the token and the previous whitespaces
	for(int i=0; i<nb_whites+1; i++){
		input.unget();
	}
	
	return tok;
}


/*******************************************count**************************************************
*
* ARGUMENTS : /
*
* RETURN : the number of read characters since the last call of reset()
*
***************************************************************************************************/
std::streamoff Lexer::count() const{
	return counter;
}


/*******************************************reset**************************************************
*
* ARGUMENTS : /
*
* RETURN : /
*
***************************************************************************************************/
void Lexer::reset(){
	counter = 0;
}


/*******************************************peek**************************************************
*
* ARGUMENTS :
*	- first : a string corresponding to the last character extracted from the input stream
*	- extract_char : a boolean varibale that takes the value "true" if we have to delete the
*	  extracted_char (next) and "false" if not (peek).
*
* RETURN : the next token of the input stream
*
***************************************************************************************************/
Lexer::token Lexer::analyze(const char& first, bool extract_char){

	Lexer::token tok;
	char second, third;
	string error;
	error += first;

	switch(first){

		//Check the "one character" tokens
		case 'x' : tok = X;
			break;
		case 'y' : tok = Y;
			break;
		case '(' : tok = OPEN_PAR;
			break;
		case ')' : tok = CLOSE_PAR;
			break;
		case '*' : tok = TIMES;
			break;
		case ',' : tok = COMMA;
			break;

		//Check the "two characters" token (pi)
		case 'p' :
			if(input.peek() == input.eof() || input.peek() == '\n'){
				throw std::domain_error("EOI");
			}else{
				second = input.get();
		
				if(second == 'i'){
					tok = PI;
				}else{
					error += second;
					throw std::domain_error("BAD TOKEN : " + error);
				}
				if(!extract_char){
					input.unget();
				}else{
					counter++;
				}
			}
			break;

		//Check the "three characters" tokens (sin, cos, avg)	
		case 's' :
		case 'c' :
		case 'a' :
			if(input.peek() == input.eof() || input.peek() == '\n'){
				throw std::domain_error("EOI");
			}else{
				second = input.get();
				error += second;
				if(input.peek() == input.eof() || input.peek() == '\n'){
					throw std::domain_error("EOI");
				}else{
					third = input.get();
					error += third;
					if(first == 's' && second == 'i' && third == 'n'){
						tok = SIN;
					}else if(first == 'c' && second == 'o' && third == 's'){
						tok = COS;
					}else if(first == 'a' && second == 'v' && third == 'g'){
						tok = AVG;
					}else{
						throw std::domain_error("BAD TOKEN : " + error);
					}
					if(!extract_char){
						input.unget();
						input.unget();
					}else{
						counter+=2;
					}
				}
			}
			break;

		// In all other cases, we throw a "bad token" domain_error
		default :
			throw std::domain_error("BAD TOKEN : " + error);
	}

	return tok;
}



/*********************************PARSER_CONSTRUCTOR**************************************
*
*****************************************************************************************/
Parser::Parser(std::istream& in): lexer(in), nb_par(0){}



/*******************************************parse**************************************************************
*
* ARGUMENTS :
*	- exp : a vector of string containing the parsed expression
*
* RETURN : returns true if it success-fully parsed a complete expression (false if the expression is incomplete)
*
****************************************************************************************************************/
bool Parser::parse(Exp& exp){

	bool success = false;

	try{
		while(true){
			success = check_syntax();
		}
	}catch(std::domain_error& e){

		string error(e.what());

		if(error != "EOI"){
			throw;
		}else{
			if(nb_par != 0){
				throw std::domain_error("PARSE ERROR at : "+ std::to_string(lexer.count()+1));
			}else{
				success = true;
			}
		}
	}

	if(success){
		infix_to_rpn(exp);
	}

	return success;
}



/*******************************************check_syntax******************************************************
*
* ARGUMENTS : /
*
* RETURN : returns true if it the syntax of the tokens is correct (false neither)
*
****************************************************************************************************************/
bool Parser::check_syntax(){

	bool success = false;
	int token_size = 0;

	Lexer::token tok = lexer.next();

	//Check x and y
	if(tok == Lexer::X || tok == Lexer::Y){
		token_vec.push_back(tok);
		success = true;

		//Check next token
		if(!check_after_token()){
			token_size = check_size_token();
			lexer.next();
			throw std::domain_error("PARSE ERROR at : "+ std::to_string(lexer.count()+1 - token_size));
		}
	}

	//Check product
	else if(tok == Lexer::OPEN_PAR){
		nb_par++;
		token_vec.push_back(tok);
		success = check_product();
	}
	
	//Check sin and cos syntax
	else if(tok == Lexer::SIN || tok == Lexer::COS){
		token_vec.push_back(tok);
		success = check_sin_cos();
	}

	//Check avg syntax
	else if(tok == Lexer::AVG){
		token_vec.push_back(tok);
		success = check_avg();
	}

	//Invalid token
	else{
		throw std::domain_error("PARSE ERROR at : "+ std::to_string(lexer.count()));
	}

	return success;
}



/*******************************************check_product******************************************************
*
* ARGUMENTS : /
*
* RETURN : returns true if the syntax of the product expression is correct (false neither)
*
****************************************************************************************************************/
bool Parser::check_product(){

	int token_size = 0;

	//If no exp1
	if(lexer.peek() == Lexer::TIMES){
		return false;
	} 
	//If exp1 invalid
	if(lexer.peek() == Lexer::CLOSE_PAR || lexer.peek() == Lexer::COMMA || lexer.peek() == Lexer::PI){
		token_size = check_size_token();
		lexer.next();
		throw std::domain_error("PARSE ERROR at : "+ std::to_string(lexer.count()+1 - token_size));
	}
	//If exp1 exists, check its syntax
	if(!check_syntax()){
		return false;
	}
	
	if(lexer.peek() != Lexer::TIMES){
		token_size = check_size_token();
		lexer.next();
		throw std::domain_error("PARSE ERROR at : "+ std::to_string(lexer.count()+1 - token_size));
	}else{
		//Add * to the token vec
		token_vec.push_back(lexer.next());
	}
	
	//If no exp2
	if(lexer.peek() == Lexer::CLOSE_PAR){
		return false;
	}
	// If exp2 invalid
	if(lexer.peek() == Lexer::TIMES || lexer.peek() == Lexer::COMMA || lexer.peek() == Lexer::PI){
		token_size = check_size_token();
		lexer.next();
		throw std::domain_error("PARSE ERROR at : "+ std::to_string(lexer.count()+1 - token_size));
	}
	//If exp2 exists, check its syntax
	if(!check_syntax()){
		return false;
	}
	
	//Check is it ends by a close par
	if(lexer.peek() != Lexer::CLOSE_PAR){
		token_size = check_size_token();
		lexer.next();
		throw std::domain_error("PARSE ERROR at : "+ std::to_string(lexer.count()+1 - token_size));
	}else{
		nb_par--;
		token_vec.push_back(lexer.next());
	}

	//Check the next token after close_par
	if(!check_after_token()){
		token_size = check_size_token();
		lexer.next();
		throw std::domain_error("PARSE ERROR at : "+ std::to_string(lexer.count()+1 - token_size));
	}

	return true;
}


/*******************************************check_sin_cos*******************************************************
*
* ARGUMENTS : /
*
* RETURN : returns true if the syntax of the sin/cos expression is correct (false neither)
*
****************************************************************************************************************/
bool Parser::check_sin_cos(){

	int token_size = 0;

	//Check if it's of type "sin(pi*exp)" or "cos(pi*exp)"
	if(lexer.peek() != Lexer::OPEN_PAR){
		token_size = check_size_token();
		lexer.next();
		throw std::domain_error("PARSE ERROR at : "+ std::to_string(lexer.count()+1 - token_size));
	}
	nb_par++;
	token_vec.push_back(lexer.next());

	if(lexer.peek() != Lexer::PI){
		token_size = check_size_token();
		lexer.next();
		throw std::domain_error("PARSE ERROR at : "+ std::to_string(lexer.count()+1 - token_size));
	}
	token_vec.push_back(lexer.next());

	if(lexer.peek() != Lexer::TIMES){
		token_size = check_size_token();
		lexer.next();
		throw std::domain_error("PARSE ERROR at : "+ std::to_string(lexer.count()+1 - token_size));
	}
	token_vec.push_back(lexer.next());

	//Check if exp exists
	if(lexer.peek() == Lexer::CLOSE_PAR){
		return false;
	}
	//Check if it is valid
	if(lexer.peek() == Lexer::TIMES || lexer.peek() == Lexer::COMMA || lexer.peek() == Lexer::PI){
		token_size = check_size_token();
		lexer.next();
		throw std::domain_error("PARSE ERROR at : "+ std::to_string(lexer.count()+1 - token_size));
	}
	//If it's valid, check it's syntax
	if(!check_syntax()){
		return false;
	}
	
	if(lexer.peek() != Lexer::CLOSE_PAR){
		token_size = check_size_token();
		lexer.next();
		throw std::domain_error("PARSE ERROR at : "+ std::to_string(lexer.count()+1 - token_size));
	}else{
		nb_par--;
		token_vec.push_back(lexer.next());
	}

	//Check the next token after close_par
	if(!check_after_token()){
		token_size = check_size_token();
		lexer.next();
		throw std::domain_error("PARSE ERROR at : "+ std::to_string(lexer.count()+1 - token_size));
	}

	return true;
}


/*******************************************check_avg************************************************************
*
* ARGUMENTS : /
*
* RETURN : returns true if the syntax of the average expression is correct (false neither)
*
****************************************************************************************************************/
bool Parser::check_avg(){

	int token_size = 0;

	if(lexer.peek() != Lexer::OPEN_PAR){
		token_size = check_size_token();
		lexer.next();
		throw std::domain_error("PARSE ERROR at : "+ std::to_string(lexer.count()+1 - token_size));
	}
	nb_par++;
	token_vec.push_back(lexer.next());

	//Check if exp1 exists
	if(lexer.peek() == Lexer::COMMA){
		return false;
	}
	//Check if exp1 is valid
	if(lexer.peek() == Lexer::TIMES || lexer.peek() == Lexer::CLOSE_PAR || lexer.peek() == Lexer::PI){
		token_size = check_size_token();
		lexer.next();
		throw std::domain_error("PARSE ERROR at : "+ std::to_string(lexer.count()+1 - token_size));
	}
	//If exp1 is valid, check it's syntax
	if(!check_syntax()){
		return false;
	}
	
	if(lexer.peek() != Lexer::COMMA){
		token_size = check_size_token();
		lexer.next();
		throw std::domain_error("PARSE ERROR at : "+ std::to_string(lexer.count()+1 - token_size));
	}else{
		//Add , to the token vec
		token_vec.push_back(lexer.next());
	}

	//Check if exp2 exists
	if(lexer.peek() == Lexer::CLOSE_PAR){
		return false;
	}
	//Check if exp2 is valid
	if(lexer.peek() == Lexer::TIMES || lexer.peek() == Lexer::COMMA || lexer.peek() == Lexer::PI){
		token_size = check_size_token();
		lexer.next();
		throw std::domain_error("PARSE ERROR at : "+ std::to_string(lexer.count()+1 - token_size));
	}
	//If exp2 is valid, check it's syntax
	if(!check_syntax()){
		return false;
	}
	
	if(lexer.peek() != Lexer::CLOSE_PAR){
		token_size = check_size_token();
		lexer.next();
		throw std::domain_error("PARSE ERROR at : "+ std::to_string(lexer.count()+1 - token_size));
	}else{
		nb_par--;
		token_vec.push_back(lexer.next());
	}

	//Check the next token after close_par
	if(!check_after_token()){
		token_size = check_size_token();
		lexer.next();
		throw std::domain_error("PARSE ERROR at : "+ std::to_string(lexer.count()+1 - token_size));
	}

	return true;
}



/*******************************************check_after_token***************************************************
*
* ARGUMENTS : /
*
* RETURN : true if the token after a close par is valid
*
****************************************************************************************************************/
bool Parser::check_after_token(){
	
	if(lexer.peek() == Lexer::TIMES || lexer.peek() == Lexer::COMMA || lexer.peek() == Lexer::CLOSE_PAR){
		return true;
	}

	return false;
}


/*******************************************check_size_token***************************************************
*
* ARGUMENTS : /
*
* RETURN : an int corresponding to the size of the next token being checked
*
****************************************************************************************************************/
int Parser::check_size_token(){
	int token_size = 0;

	if(lexer.peek() == Lexer::COS || lexer.peek() == Lexer::SIN || lexer.peek() == Lexer::AVG)
		token_size = 3;
	else if(lexer.peek() == Lexer::PI)
		token_size = 2;
	else
		token_size = 1;

	return token_size;
}

/*******************************************infix_to_rpn*********************************************************
*
* ARGUMENTS : 
*	- exp : a vector of string containing the reverse polish notation of the given sequence
*
* RETURN : /
*
****************************************************************************************************************/
void Parser::infix_to_rpn(Exp& exp){

	Lexer::token tok;
	Exp operator_stack;

	//While tokens are read from the lexer
	for(vector<Lexer::token>::size_type i=0; i != token_vec.size(); i++){

		tok = token_vec[i];

		//If token is a number "x,y,pi", then push it onto exp
		if(tok == Lexer::X || tok == Lexer::Y || tok == Lexer::PI){
			exp.push_back(token_to_string(tok));
		}

		//If AVG is met
		else if(tok == Lexer::AVG){
			//Push the read operator onto the operator stack
			operator_stack.push_back(token_to_string(tok));
		}

		//If it's a comma
		else if(tok == Lexer::COMMA){
			//While there is an operator at the top of the operator_stack with greater or equal precedence
			while(greater_precedence(tok, operator_stack.back())){
			
				//Pop operators from the operator stack and push them onto exp
				exp.push_back(operator_stack.back());
				operator_stack.pop_back();

				if(operator_stack.empty()){
					break;
				}
			}
		}

		//If token is an operator "*,sin,cos", then:
		else if(tok == Lexer::SIN || tok == Lexer::COS || tok == Lexer::TIMES){

			if(!operator_stack.empty()){
				//While there is an operator at the top of the operator_stack with greater or equal precedence
				while(greater_precedence(tok, operator_stack.back())){
				
					//Pop operators from the operator stack and push them onto exp
					exp.push_back(operator_stack.back());
					operator_stack.pop_back();

					if(operator_stack.empty()){
						break;
					}
				}
			}
			//Push the read operator onto the operator stack
			operator_stack.push_back(token_to_string(tok));
		}

		//If it's a left bracket "(", then push it onto the operator stack
		else if(tok == Lexer::OPEN_PAR){
			operator_stack.push_back(token_to_string(tok));
		}

		//If it's a right bracket ")", then :
		else if(tok == Lexer::CLOSE_PAR){

			//While there is no left bracket at the top of the operator stack:
			while(operator_stack.back() != "("){

				//Pop operators from the operator stack and push them onto exp
				exp.push_back(operator_stack.back());
				operator_stack.pop_back();

			}
			//Pop the left bracket from the operator stack and delete it
			operator_stack.pop_back();
		}
	}

	//While there are operators on the operator stack, pop them onto exp
	while(!operator_stack.empty()){
		exp.push_back(operator_stack.back());
		operator_stack.pop_back();
	}
}


/*******************************************token_to_string*****************************************************
*
* ARGUMENTS : 
*	- tok : the token to convert in string
*
* RETURN : the corresponding string
*
****************************************************************************************************************/
string Parser::token_to_string(const Lexer::token& tok){
	string str = "";

	if(tok == Lexer::X){
		str = "x";
	}else if(tok == Lexer::Y){
		str = "y";
	}else if(tok == Lexer::PI){
		str = "pi";
	}else if(tok == Lexer::SIN){
		str = "sin";
	}else if(tok == Lexer::COS){
		str = "cos";
	}else if(tok == Lexer::OPEN_PAR){
		str = "(";
	}else if(tok == Lexer::AVG){
		str = "avg";
	}else if(tok == Lexer::TIMES){
		str = "*";
	}

	return str;
}


/*******************************************greater_precedence***************************************************
*
* ARGUMENTS : 
*	- tok : the read token to be compared with the top of the operator stack (str)
*	- str : the string expression at the top of the operator stack
*
* RETURN : returns true if the string is of greater or equal precedence than the token (false neither)
*
****************************************************************************************************************/
bool Parser::greater_precedence(const Lexer::token& tok, const string& str){

	if(tok == Lexer::COMMA){
		if(str == "avg" || str == "sin" || str == "cos"){
			return true;
		}
	}
	else if(tok == Lexer::TIMES){
		if(str == "*" || str == "sin" || str == "cos"){
			return true;
		}
	}else if(tok == Lexer::SIN || tok == Lexer::COS){
		if(str == "sin" || str == "cos"){
			return true;
		}
	}

	return false;
}
