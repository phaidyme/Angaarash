#pragma once

#include <optional>
#include <queue>
#include <memory>
#include <unordered_set>

#include "Token.hpp"

class Calculator {
	std::unordered_set<Function> functions;
public:
	Calculator();

	std::unordered_set<Function> get_functions();

	static std::optional<std::queue<std::shared_ptr<Token>>>
		shunting_yard(std::vector<std::shared_ptr<Token>>);
	static std::optional<Number>
		evaluate_postfix_expression(std::queue<std::shared_ptr<Token>>);
};