#include <cassert>
#include <cmath>

#include <stdexcept>
#include <unordered_map>

#include "Token.hpp"

/*                                NUMBER                                      */
std::optional<Number> Number::parse(std::string& input) {
	if(!isdigit(input[0])) return std::nullopt;

	// count digits + possible decimal point
	uint n = 0;
	bool passed_decimal = false;
	while (n < input.length()) {
		if (isdigit(input[n])) {
			n++;
		}
		else {
			if (passed_decimal) {
				break;
			}
			else {
				if (input[n] == '.') {
					passed_decimal = true;
					n++;
				}
				else {
					break;
				}
			}
		}
	}
	double number_val = atof(input.c_str());

	input.erase(0, n);

	return number_val;
}

/*                                 FUNCTION                                   */
const std::unordered_map<
	std::string,
	std::function<Number(Number)>
> Function::map = {
	{"sin", sin}, {"cos", cos}, {"tan", tan},
	{"arcsin", asin}, {"arccos", acos}, {"arctan", atan}
};
Function::Function(std::string s_type) {
	assert(map.contains(s_type));
	type = map.at(s_type);
};
Number Function::operator() (Number number) {
	return type(number);
}

std::optional<Function> Function::parse(std::string& input) {
	for(auto const& entry: map) {
		if (input.find(entry.first) == 0) {
			input.erase(0, entry.first.length());
			return Function(entry.first);
		}
	}
	return std::nullopt;
}

const Operator::Type Operator::types[5] = {
	addition,subtraction,multiplication,division,exponentiation
};

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