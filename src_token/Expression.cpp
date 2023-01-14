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

void Expression::push(TokenPtr tp) {
	expression.push_back(tp);
}

bool Expression::is_empty() {
	return expression.empty();
}

std::vector<TokenPtr>::iterator Expression::begin() {
	return expression.begin();
}

std::vector<TokenPtr>::iterator Expression::end() {
	return expression.end();
}