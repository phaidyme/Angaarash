#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <optional>
#include <memory>
#include <set>

class Token {
public:
	virtual bool is_type(const std::string&) const = 0;
	virtual operator std::string() const { return "token"; };
};
typedef std::shared_ptr<Token> TokenPtr; // maybe make this a class later?

/*                                  NUMBER                                    */
class Number: public Token {
public:
	double value;

	Number(double a_value): value(a_value) {}
	operator double() { return value; }

	bool is_type(const std::string& type_name) const override { return type_name == "Number"; }
	operator std::string() const override { return std::to_string(value); }

	static std::optional<Number> parse(std::string&);

	bool operator==(const Number&) const;
	bool operator<(const Number&) const;
};
/*                                 VARIABLE                                   */
class Variable: public Token {
public:
	std::string name;
	std::optional<Number> value;

	Variable(const std::string&);
	Variable(const std::string&, Number);
	Variable(const Variable&);

	bool is_type(const std::string&) const override;
	operator std::string() const override;
};
/*                                 OPERATOR                                   */
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

	Operator(char);

	Operator(Operator::Type a_type): type(a_type) {}

	static bool is_operator(char);
	bool is_type(const std::string& type_name) const override { return type_name == "Operator"; }
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
	bool is_type(const std::string& type_name) const override { return type_name == "LeftParenthesis"; }
	operator std::string() const override;
};
class RightParenthesis: public Token {
	bool is_type(const std::string& type_name) const override { return type_name == "RightParenthesis"; }
	operator std::string() const override;
};