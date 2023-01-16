#pragma once

#include <optional>
#include <queue>
#include <memory>
#include <map>
#include <unordered_set>

#include "helper_functions.hpp"

#include "src_token/Token.hpp"
#include "src_token/Function.hpp"
	
// Yeah it's a singleton, sue me.
class Calculator {
	Calculator();
	static std::optional<std::queue<std::shared_ptr<Token>>>
		shunting_yard(Expression);
	static std::optional<Number>
		evaluate_postfix_expression(std::queue<std::shared_ptr<Token>>);
public:
	std::map<std::string,Function> functions;
	std::map<std::string,Number,string_length_comp> variables;
	std::unordered_set<std::string> arguments;
	static Calculator& get_instance();
	std::optional<Number> evaluate(Expression&);
	Calculator(const Calculator&) = delete;
	void operator=(const Calculator&) = delete;
};