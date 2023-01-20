#pragma once

#include <vector>

#include "Token.hpp"

class Expression: public Token {
public:
	std::vector<TokenPtr> expression;
	
	Expression();
	
	bool is_type(const std::string& type_name) const override;
	operator std::string() const override;

	void push(TokenPtr);
	TokenPtr operator [] (std::size_t);
	bool is_empty() const;
	std::size_t length() const;
	std::vector<TokenPtr>::iterator begin();
	std::vector<TokenPtr>::iterator end();
	std::vector<TokenPtr>::const_iterator begin() const;
	std::vector<TokenPtr>::const_iterator end() const;
};