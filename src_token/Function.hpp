#pragma once

#include "Token.hpp"
#include "Expression.hpp"

class BasicFunction: public Token {
public: std::string name;
private: double (*function)(double);
public:
	BasicFunction(std::string, double(*)(double));

	bool is_type(const std::string&) const override;
	operator std::string() const override;
	
	Number operator() (Number n) const;
};

class Function: public Token {
	std::string name;
	Variable argument;
	Expression expression;

	Function(Variable);
public:
	Function(std::string, Variable, Expression);
	Function(BasicFunction, Variable);


	bool is_type(const std::string& type_name) const override;
	operator std::string() const override;

	std::optional<Number> operator() (Number);

	std::string get_signature() const;
	std::string get_expression() const;
};