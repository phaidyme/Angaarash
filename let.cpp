#include "cpp_utils/src/String.h"

#include "evaluator.h"

#include "let.h"

void let(String & input) {
	auto post_fix = shunting_yard(input);
	std::cout << evaluate_postfix_expression(post_fix) << std::endl;
}