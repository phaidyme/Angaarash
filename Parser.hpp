#pragma once

#include <cassert>

#include <string>

#include "src_token/Token.hpp"
#include "src_token/Expression.hpp"
#include "src_token/Function.hpp"

#include "Calculator.hpp"

template<typename T>
std::optional<T> parse(std::string&);