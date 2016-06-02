#include <iostream>
#include "EraCalc.h"

int main (int argc, char const* argv[]) {
	EraCalc mtx;
	double a = mtx.Evaluate("5 + 4 - 5.2 - 4 / 2 ^ 3 + (4.8 + (6 + 1) * 5.2)");
	
	std::cout.precision(12);
	std::cout << "Value: " << a << std::endl;
	return 0;
}
