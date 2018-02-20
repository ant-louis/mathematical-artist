#include "martist.hpp"

#include <iostream>
#include <time.h>

//!!!!!!!!!!!!!!!!!!!!!!!      A ENLEVER        !!!!!!!!!!!!!!!!!!!!!!!
#include "CImg.h"
using namespace cimg_library;
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

using std::cout;
using std::cin;
using std::endl;



int main(){


	size_t width, height;
	int rdepth = 8;
	int gdepth = 7;
	int bdepth = 6;

	cout << "\n" <<"Enter the width : ";
	cin >> width;
	cout << "\n" << "Enter the height : ";
	cin >> height;

	unsigned char* buffer = (unsigned char*) malloc(3*width*height*sizeof(unsigned char));

	/*cout << "\n" <<"Enter the red depth : ";
	cin >> rdepth;
	cout << "\n" << "Enter the green depth : ";
	cin >> gdepth;
	cout << "\n" << "Enter the blue depth : ";
	cin >> bdepth;*/

	//Clear the input stream
	cin.clear();

	
	Martist martist(buffer,height,width,rdepth,gdepth,bdepth);

	//Seed the randomness
	martist.seed((int)time(NULL));

	//User input
	try{
	 	cout << "\n" <<"Enter the desired expressions : " << endl;
		cin >> martist;
        CImg<unsigned char> boardA(buffer,3,(unsigned int)width,(unsigned int)height);
        boardA.permute_axes("yzcx");
        boardA.display("Martist");
        cout << martist;
    }
    catch(std::domain_error e){
        std::cout << e.what() << std::endl;
    }
	

	/*//Random
	 try{
	 	//Random 1
		martist.paint();
        CImg<unsigned char> boardA(buffer,3,(unsigned int)width,(unsigned int)height);
        boardA.permute_axes("yzcx");
        boardA.display("Martist");
        cout << martist;
        //Random 2
        martist.paint();
        CImg<unsigned char> boardB(buffer,3,(unsigned int)width,(unsigned int)height);
        boardB.permute_axes("yzcx");
        boardB.display("Martist");
        cout << martist;
        //Random 3
        martist.paint();
        CImg<unsigned char> boardC(buffer,3,(unsigned int)width,(unsigned int)height);
        boardC.permute_axes("yzcx");
        boardC.display("Martist");
        cout << martist;
    }
    catch(std::domain_error e){
        std::cout << e.what() << std::endl;
    }*/



	free(buffer);


	return 0;
}