#include "helper_functions.hpp"

#include "Parser.hpp"
#include <iostream>

template<>
std::optional<Number>
parse<Number>(std::string& input) {
	std::size_t i;
	try {
		auto retval = std::stod(input, &i);
		input.erase(0, i);
		return retval;
	} catch(std::invalid_argument& e1) {
		return std::nullopt;
	} catch(std::out_of_range& e2) {
		return std::nullopt;
	}
}

template<>
std::optional<Variable>
parse<Variable>(std::string& input) {
	Calculator& calculator = Calculator::get_instance();
	for(const auto& x: calculator.variables) {
		if(input.find(x.first) == 0) {
			input.erase(0, x.first.length());
			return Variable(x.first, x.second);
		}
	}
	for(const auto& x: calculator.arguments) {
		if(input.find(x) == 0) {
			input.erase(0, x.length());
			return Variable(x);
		}
	}
	return std::nullopt;
}

template<>
std::optional<Function>
parse<Function>(std::string & input) {
	std::string token = "";
	for(std::size_t i=0; i<input.length() && std::isalpha(input[i]); i++) {
		token.push_back(input[i]);
	}
	Calculator& calculator = Calculator::get_instance();
	if (calculator.functions.contains(token)) {
		input.erase(0, token.length());
		return calculator.functions.at(token);
	}
	else {
		return std::nullopt;
	}
	/*
	for(auto const& f: calculator.functions) {
		if (input.find(f.first) == 0) {
			input.erase(0, f.first.length());
			return f.second;
		}
	}
	*/
}

// warning: never returns Operator::addition or Operator::subtraction
template<>
std::optional<TokenPtr>
parse<TokenPtr>(std::string& input) {
	if (auto x = parse<Number>(input)) {
		return std::make_shared<Number>(*x);
	}
	if (auto x = parse<Variable>(input)) {
		return std::make_shared<Variable>(*x);
	}
	if (auto x = parse<Function>(input)) {
		return std::make_shared<Function>(*x);
	}
	if (Operator::is_operator(input[0])) {
		char c = input[0];
		input.erase(0, 1);
		return std::make_shared<Operator>(c);
	}
	if (input[0] == '(') {
		input.erase(0, 1);
		return std::make_shared<LeftParenthesis>();
	}
	if (input[0] == ')') {
		input.erase(0, 1);
		return std::make_shared<RightParenthesis>();
	}
	return std::nullopt;
}

template<>
std::optional<Expression>
parse<Expression>(std::string& input) {
	std::optional<TokenPtr> token;
	Expression retval;

	// the first time is always special ;)
	trim_leading<' '>(input);
	if (!input.empty()) {
		token = parse<TokenPtr>(input);
		if (token) retval.push(*token);
		else return std::nullopt;
	}
	else return std::nullopt;

	trim_leading<' '>(input);
	while(!input.empty()) {
		bool
			add = input[0] == '+',
			sub = input[0] == '-',
			var = (*token)->is_type("Variable"),
			num = (*token)->is_type("Number"),
			lpar = (*token)->is_type("LeftParenthesis"),
			op = (*token)->is_type("Operator");

		if ((add || sub) && !(lpar || op)) {
			token = std::make_shared<Operator>(input[0]);
			input.erase(0, 1);
		}
		else token = parse<TokenPtr>(input);

		if ((num || var) && (
			(*token)->is_type("LeftParenthesis") ||
			(*token)->is_type("Function") ||
			(*token)->is_type("Variable")
		)) {
			retval.push(std::make_shared<Operator>(Operator::multiplication));
		}

		if (token) retval.push(*token);
		else return std::nullopt;

		trim_leading<' '>(input);
	}
	return retval;
}