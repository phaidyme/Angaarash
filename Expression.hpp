#pragma once

#include "Token.hpp"

class Expression: public Token {
	std::vector<TokenPtr> expression;
public:
	Expression();
	
	bool is_type(const std::string& type_name) const override;
	operator std::string() const override;

	void push(TokenPtr);
	bool is_empty();
	std::vector<TokenPtr>::iterator begin();
	std::vector<TokenPtr>::iterator end();
};