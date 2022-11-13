#include <iostream>

#include "cpp_utils/src/String.h"

#include "Calculator.hpp"

#define print(x) std::cout << x << std::endl

void obey(String);

int main() {
	print("Welcome to BarZamSr's low-budget calculator!");
	print("Execute any command to start. For more help, type \"help\".");

	Calculator calculator;
	std::string input;
	while (true) {
		std::getline(std::cin, input);
		if (input == "exit" || input == "quit" || input == "q") break;
		
		calculator.process_command(input);
	}

	return 0;
}