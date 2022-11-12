#include <iostream>

#include "cpp_utils/src/String.h"

#include "let.h"

#define print(x) std::cout << x << std::endl

void obey(String);

int main() {
	print("Welcome to BarZamSr's low-budget calculator!");
	print("Execute any command to start. For more help, type \"help\".");

	std::string command;
	while (true) {
		std::getline(std::cin, command);
		if (command == "exit" || command == "quit" || command == "q") break;
		
		obey(command);
	}

	return 0;
}

void obey(String command) {
	String command_type = command.slice(0, command.find(' '));
	String command_content = command.slice(command.find(' '), command.get_len());
	
	command_content = command_content.split(' ').join();
	assert(command_content.find(' ') == ERR_VAL);
	
	if (command_type == "let") {
		let(command_content);
	}
}