#include <cassert>
#include <cmath>

#include <stdexcept>
#include <unordered_map>

#include "Token.hpp"

/*                                VARIABLE                                    */
Variable::Variable(const std::string& n): name(n), value(std::nullopt) {}
Variable::Variable(const std::string& n, Number x): name(n), value(x) {}
Variable::Variable(const Variable& x): name(x.name), value(x.value) {}

bool Variable::is_type(const std::string& type_name) const {
	return type_name == "Variable";
}
Variable::operator std::string() const {
	if (value) {
		return name + ':' + (std::string)(*value);
	}
	else {
		return name;
	}
}
/*                                 NUMBER                                     */
bool Number::is_type(const std::string& t) const {
	return t == "Number";
}
bool Number::operator==(const Number& x) const {
	return value == x.value;
}
bool Number::operator<(const Number& x) const {
	return value < x.value;
}
/*                                 OPERATOR                                   */
const Operator::Type Operator::types[5] = {
	addition,subtraction,multiplication,division,exponentiation
};

Operator::Operator(char c): type(static_cast<Operator::Type>(c)) {
	if (!is_operator(c)) {
		throw std::runtime_error("Operator::Operator(char): invalid operation");
	}
}

Operator::operator std::string() const {
	std::string retval;
	retval.push_back(type);
	return retval;
}

bool Operator::is_operator(char c) {
	for (auto t: Operator::types) {
		if (c == t) return true;
	}
	return false;
}

double Operator::evaluate(Operator const& o, double a, double b) {
	if (o.type == addition) return a + b;
	else if (o.type == subtraction) return a - b;
	else if (o.type == multiplication) return a * b;
	else if (o.type == division) return a / b;
	else if (o.type == exponentiation) return pow(a, b);
	else throw std::runtime_error("Operator::evaluate(): invalid operation");
}

bool operator==(Operator const& lhs, Operator const& rhs) {
	std::unordered_map<Operator::Type,int> priority = {{
		{Operator::addition, 1}, {Operator::subtraction, 1},
		{Operator::multiplication, 2}, {Operator::division, 2},
		{Operator::exponentiation, 3}
	}};
	return priority[lhs.type] == priority[rhs.type];
}
bool operator!=(Operator const& lhs, Operator const& rhs) { return !(lhs == rhs); }
bool operator< (Operator const& lhs, Operator const& rhs) {
	std::unordered_map<Operator::Type,int> priority = {{
		{Operator::addition, 1}, {Operator::subtraction, 1},
		{Operator::multiplication, 2}, {Operator::division, 2},
		{Operator::exponentiation, 3}
	}};
	return priority[lhs.type] < priority[rhs.type];
}
bool operator> (Operator const& lhs, Operator const& rhs) { return  rhs < lhs; }
bool operator<=(Operator const& lhs, Operator const& rhs) { return !(lhs > rhs); }
bool operator>=(Operator const& lhs, Operator const& rhs) { return !(lhs < rhs); }

/* BRACKETS */
LeftParenthesis::operator std::string() const {
	return "(";
}
RightParenthesis::operator std::string() const {
	return ")";
}