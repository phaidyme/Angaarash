#pragma once

#include <unordered_map>
#include <stack>
#include <queue>
#include <memory>

#include "Token.hpp"

class Calculator {
public:
	Calculator() {};
	void process_command(std::string);
	double evaluate_postfix_expression(std::queue<std::shared_ptr<Token>>);
	std::queue<std::shared_ptr<Token>> shunting_yard(std::string);
	std::shared_ptr<Token> read_token(std::string &);
	double read_number(std::string &);
};