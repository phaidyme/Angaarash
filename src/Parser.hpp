#pragma once

#include <cassert>

#include <string>

#include "Token.hpp"
#include "Expression.hpp"
#include "Function.hpp"

#include "Calculator.hpp"

template<typename T>
std::optional<T> parse(std::string&);