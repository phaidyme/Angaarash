#ifndef TOKEN
#define TOKEN

#include <unordered_map>

class Token {
public:
	virtual bool is_type(String) const = 0;
	virtual operator String() const { return ""; };
};

class Number: public Token {
public:
	double value;

	Number(double a_value): value(a_value) {}
	operator double() { return value; }

	bool is_type(String type_name) const override { return type_name == "Number"; }
	operator String() const override { return std::to_string(value); }
};

class Function: public Token {
public:
	bool is_type(String type_name) const override { return type_name == "Function"; }
	static double evaluate(Function const&, double);
};

class Operator: public Token {
public:
	enum Type {addition='+', subtraction='-', multiplication='*', division='/', exponentiation='^'};
	static const Type types[5];
	Type type;

	Operator(char c): type(static_cast<Operator::Type>(c)) {}

	Operator(Operator::Type a_type): type(a_type) {}

	static bool is_operator(char);
	bool is_type(String type_name) const override { return type_name == "Operator"; }
	operator String() const override;

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
	bool is_type(String type_name) const override { return type_name == "LeftParenthesis"; }
};
class RightParenthesis: public Token {
	bool is_type(String type_name) const override { return type_name == "RightParenthesis"; }
};

#endif