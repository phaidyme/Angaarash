#pragma once

#include <optional>
#include <queue>
#include <memory>
#include <unordered_set>

#include "Token.hpp"

class Calculator {
	static std::optional<std::queue<std::shared_ptr<Token>>>
		shunting_yard(std::vector<std::shared_ptr<Token>>);
	static std::optional<Number>
		evaluate_postfix_expression(std::queue<std::shared_ptr<Token>>);
public:
	std::unordered_map<std::string,std::function<Number(Number)>> functions;
	std::unordered_map<std::string,Number> variables;
	Calculator();

	std::optional<Number> evaluate(const std::vector<std::shared_ptr<Token>>&);
};