#include <cassert>
#include <cmath>

#include <iostream>

#include "helper_functions.hpp"
#include "Calculator.hpp"

#include "Function.hpp"

/* BASIC FUNCTION */
BasicFunction::BasicFunction(std::string n, double(*f)(double)):
name(n), function(f) {
	// :)
}
bool BasicFunction::is_type(const std::string& type_name) const {
	return type_name == "BasicFunction";
}
BasicFunction::operator std::string() const {
	return name;
}
Number BasicFunction::operator() (Number n) const {
	return function(n);
}

/* FUNCTION */
Function::Function(Variable arg): argument(arg) {}
Function::Function(std::string n, Variable arg, Expression e):
name(n), argument(arg), expression(e) {
	for(auto& token: expression.expression) {
		if (token->is_type("Variable")) {
			auto x = std::static_pointer_cast<Variable>(token);
			if (x->name == argument.name) {
				std::static_pointer_cast<Variable>(token)->value = std::nullopt;
			}
		}
	}
}
Function::Function(BasicFunction f, Variable arg):
name(f.name), argument(arg) {
	expression.expression.push_back(std::make_shared<BasicFunction>(f));
	expression.expression.push_back(std::make_shared<LeftParenthesis>());
	expression.expression.push_back(std::make_shared<Variable>(arg));
	expression.expression.push_back(std::make_shared<RightParenthesis>());
}

bool Function::is_type(const std::string& type_name) const {
	return type_name == "Function";
}
Function::operator std::string() const {
	return name;
}
std::optional<Number> Function::operator() (Number n) {
	Calculator& calculator = Calculator::get_instance();
	// substitute n
	for(auto const& token: expression.expression) {
		if (token->is_type("Variable")) {
			auto x = std::static_pointer_cast<Variable>(token);
			if (x->name == argument.name) {
				x->value = n;
			}
		}
	}
	auto retval = calculator.evaluate(expression);
	// unsubstitute n
	for(auto const& token: expression.expression) {
		if (token->is_type("Variable")) {
			auto x = std::static_pointer_cast<Variable>(token);
			if (x->name == argument.name) {
				std::static_pointer_cast<Variable>(token)->value = std::nullopt;
			}
		}
	}
	return retval;
}
std::string Function::get_signature() const {
	return name + '(' + argument.name + ')';
}
std::string Function::get_expression() const {
	std::string retval = "";
	for(const auto& token: expression.expression) {
		retval.append(token->operator std::string());
	}
	return retval;
}