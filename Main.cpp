#include <iostream>
#include "EraCalc.h"

int main (int argc, char const* argv[]) {
	EraCalc mtx;
	double res = 0;
	std::cout << "Press CTRL+C to exit." << std::endl;
	while(true) {
		std::cout << ">> " << std::flush;
		std::string line;
		getline(std::cin, line);
		if(mtx.Evaluate(line.c_str(), &res) == EraCalc::Failed){
			std::cout << "error: " << mtx.GetErrorMessage() << std::endl;
		} else {
			std::cout.precision(12);
			std::cout << res << std::endl;
		}
	}
	return 0;
}  
