#pragma once


#include <unordered_map>
#include <stack>
#include <queue>
#include <memory>

#include "cpp_utils/src/String.h"

#include "token.h"

/*
The shunting yard algorithm is an operator precedence parser, which converts the common
infix notation to PPE (e.g. "3 + 4" to "3 4 +").
This is useful because expressions in PPE can be evaluated without need for brackets or
an understanding of order of operations such as BODMAS or PEMDAS.
*/
std::queue<std::shared_ptr<Token>> shunting_yard(String);
std::shared_ptr<Token> read_token(String &);
double read_number(String &);

double evaluate_postfix_expression(std::queue<std::shared_ptr<Token>>);