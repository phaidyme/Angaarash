#include <cassert>

#include <stdexcept>

#include "Token.hpp"

double Function::evaluate(Function const& f, double number) {
	assert(!"todo: implement");
	return 0;
}

const Operator::Type Operator::types[5] = {addition,subtraction,multiplication,division,exponentiation};

Operator::operator String() const {
	String retval;
	retval.push(type);
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
	else throw std::runtime_error("invalid operation");
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