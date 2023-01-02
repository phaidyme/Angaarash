#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <optional>
#include <memory>
#include <set>

class Token {
public:
	virtual bool is_type(std::string) const = 0;
	virtual operator std::string() const { return "token"; };
};

class Number: public Token {
public:
	double value;

	Number(double a_value): value(a_value) {}
	operator double() { return value; }

	bool is_type(std::string type_name) const override { return type_name == "Number"; }
	operator std::string() const override { return std::to_string(value); }

	static std::optional<Number> parse(std::string&);
};

class Function: public Token {
	static const std::unordered_map<
		std::string,
		std::function<Number(Number)>
	> map;
	std::function<Number(Number)> type;
public:
	Function(std::string);
	bool is_type(std::string type_name) const override { return type_name == "Function"; }
	Number operator() (Number);

	static std::optional<Function> parse(std::string&);
};

class Operator: public Token {
public:
	enum Type {
		addition='+',
		subtraction='-',
		multiplication='*',
		division='/',
		exponentiation='^'
	};
	
	static const Type types[5];
	Type type;

	Operator(char c): type(static_cast<Operator::Type>(c)) {}

	Operator(Operator::Type a_type): type(a_type) {}

	static bool is_operator(char);
	bool is_type(std::string type_name) const override { return type_name == "Operator"; }
	operator std::string() const override;

	bool is_left_associative() {
		return this->type != exponentiation;
	}

	static double evaluate(Operator const&,double,double);
};
bool operator==(Operator const&, Operator const&);
bool operator!=(Operator const&, Operator const&);
bool operator< (Operator const&, Operator const&);
bool operator> (Operator const&, Operator const&);
bool operator<=(Operator const&, Operator const&);
bool operator>=(Operator const&, Operator const&);

class LeftParenthesis: public Token {
	bool is_type(std::string type_name) const override { return type_name == "LeftParenthesis"; }
};
class RightParenthesis: public Token {
	bool is_type(std::string type_name) const override { return type_name == "RightParenthesis"; }
};