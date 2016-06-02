#include <iostream>
#include "EraCalc.h"

int main (int argc, char const* argv[]) {
	EraCalc mtx;
	double a = 0;
	if(mtx.Evaluate("5 + 4 - 5.2 - 4 / 2 ^ 3 + (4.8 + (6 + 1) * 5.2)", &a) == EraCalc::Failed){
		std::cout << mtx.GetErrorMessage() << std::endl;
	} else {
		std::cout.precision(12);
		std::cout << "Value: " << a << std::endl;
	}
	return 0;
}  
