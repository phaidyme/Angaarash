#include "Expression.hpp"

Expression::Expression() {
	// :)
}

bool Expression::is_type(const std::string& type_name) const {
	return type_name == "Expression";
}
Expression::operator std::string() const {
	std::string retval = "";
	for(const auto& token: expression) {
		retval.append((std::string)(*token));
		retval.append(" ");
	}
	return retval;
}

void Expression::push(TokenPtr token) {
	expression.push_back(token);
}
TokenPtr Expression::operator [] (std::size_t i) {
	return expression[i];
}
bool Expression::is_empty() const {
	return expression.empty();
}
std::size_t Expression::length() const {
	return expression.size();
}
std::vector<TokenPtr>::iterator Expression::begin() {
	return expression.begin();
}
std::vector<TokenPtr>::iterator Expression::end() {
	return expression.end();
}
std::vector<TokenPtr>::const_iterator Expression::begin() const {
	return expression.cbegin();
}
std::vector<TokenPtr>::const_iterator Expression::end() const {
	return expression.cend();
}