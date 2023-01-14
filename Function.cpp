#include <cassert>
#include <cmath>

#include <iostream>

#include "helper_functions.hpp"
#include "Function.hpp"
#include "Calculator.hpp"

/* BASIC FUNCTION */
BasicFunction::BasicFunction(double(*f)(double)): function(f) {}
bool BasicFunction::is_type(const std::string& type_name) const {
	return type_name == "BasicFunction";
}
Number BasicFunction::operator() (Number n) const {
	return function(n);
}

/* FUNCTION */
Function::Function(Variable arg): argument(arg) {}
Function::Function(Variable arg, Expression e): argument(arg), expression(e) {}
Function::Function(BasicFunction f, Variable arg): argument(arg) {
	expression.push(std::make_shared<BasicFunction>(f));
	expression.push(std::make_shared<LeftParenthesis>());
	expression.push(std::make_shared<Variable>(arg));
	expression.push(std::make_shared<RightParenthesis>());
}

bool Function::is_type(const std::string& type_name) const {
	return type_name == "Function";
}
Function::operator std::string() const {
	return expression;
}
std::optional<Number> Function::operator() (Number n) {
	Calculator& calculator = Calculator::get_instance();
	// substitute n
	for(auto const& token: expression) {
		if (token->is_type("Variable")) {
			auto x = std::static_pointer_cast<Variable>(token);
			if (x->name == argument.name) {
				x->value = n;
			}
		}
	}
	return calculator.evaluate(expression);
}