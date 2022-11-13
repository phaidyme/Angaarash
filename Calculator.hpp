#pragma once

#include <unordered_map>
#include <stack>
#include <queue>
#include <memory>

#include "Token.hpp"

class Calculator {
public:
	void process_command(String);
private:
	void let(String &);
};

double evaluate_postfix_expression(std::queue<std::shared_ptr<Token>>);
std::queue<std::shared_ptr<Token>> shunting_yard(String);
std::shared_ptr<Token> read_token(String &);
double read_number(String &);