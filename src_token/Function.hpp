#pragma once

#include "Token.hpp"
#include "Expression.hpp"

class BasicFunction: public Token {
	double (*function)(double);
public:
	BasicFunction(double(*)(double));

	bool is_type(const std::string&) const override;
	
	Number operator() (Number n) const;
};

class Function: public Token {
	Variable argument;
	Expression expression;

	Function(Variable);
public:
	Function(Variable, Expression);
	Function(BasicFunction, Variable);


	bool is_type(const std::string& type_name) const override;
	operator std::string() const override;

	std::optional<Number> operator() (Number);
};