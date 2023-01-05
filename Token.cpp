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
	return name;
}

bool Variable::is_known() const {
	return value.has_value();
}
Variable::operator Number() const {
	assert(this->is_known());
	return *(this->value);
}
Variable Variable::operator= (const Number& x) {
	value = x;
	return *this;
}
Variable Variable::operator= (const Variable& x) {
	// TODO: make x = y without knowing what y is (maybe shared_ptr instead of optional?)
	assert(x.value);
	value = x.value;
	return *this;
}
bool Variable::operator== (const Number& x) const {
	assert(value);
	return *value == x;
}
bool Variable::operator== (const Variable& x) const {
	assert(x.value);
	return this->operator==(*x.value);
}
bool Variable::operator< (const Number& x) const {
	assert(value);
	return *value < x;
}
bool Variable::operator< (const Variable& x) const {
	assert(x.value);
	return this->operator<(*x.value);
}
std::size_t std::hash<Variable>::operator()(const Variable& x) const {
	return hash<string>()(x);
}
/*                                 NUMBER                                     */
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

bool Number::operator==(const Number& x) const {
	return value == x.value;
}
bool Number::operator<(const Number& x) const {
	return value < x.value;
}

/*                                 FUNCTION                                   */
Function::Function(std::string n, double f(double)): name(n), func(f) {}

bool Function::is_type(const std::string& type_name) const {
	return type_name == "Function";
}
Function::operator std::string() const {
	return name;
}
Number Function::operator() (Number n) {
	return func(n);
}
bool Function::operator==(const Function& other) const {
	return name == other.name;
}

std::size_t std::hash<Function>::operator()(const Function & f) const {
	return hash<string>()(std::string(f));
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