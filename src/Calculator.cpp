#define _USE_MATH_DEFINES
#include <cmath>

#include <stack>
#include <iostream>

#include "helper_functions.hpp"
#include "Calculator.hpp"

Calculator& Calculator::get_instance() {
	static Calculator calculator;
	return calculator;
}

Calculator::Calculator() {
	// I've rearranged this data so many times, might as well
	std::vector<double(*)(double)> default_functions = {
		sin, cos, tan,
		asin, acos, atan
	};
	std::vector<std::string> default_function_names = {
		"sin", "cos", "tan",
		"arcsin", "arccos", "arctan"
	};
	std::vector<std::string> default_argument_names {
		"theta", "theta", "theta",
		"x", "x", "x"
	};
	for(std::size_t i=0; i<6; i++) {
		functions.emplace(
			default_function_names[i],
			Function(
				BasicFunction(default_function_names[i], default_functions[i]),
				Variable(default_argument_names[i])
			)
		);
	}
	
	variables.emplace("pi", M_PI);
	variables.emplace("e", M_E);
}

std::optional<Number> Calculator::evaluate(Expression& expression) {
	if (expression.expression.empty()) {
		return std::nullopt;
	}
	// make sure all Variables are known and convert them to Numbers
	Expression exp_copy = {};
	for(const auto& token: expression.expression) {
		if(token->is_type("Variable")) {
			Variable x = *std::static_pointer_cast<Variable>(token);
			if(!(x.value)) {
				return std::nullopt;
			}
			exp_copy.expression.push_back(std::make_shared<Number>(*(x.value)));
		}
		else {
			exp_copy.expression.push_back(token);
		}
	}

	if(auto rpn_exp = shunting_yard(exp_copy)) {
		return evaluate_postfix_expression(*rpn_exp);
	}
	else {
		return std::nullopt;
	}
}

std::optional<Number> Calculator::evaluate_postfix_expression(
	std::queue<std::shared_ptr<Token>> expression
) {
	std::shared_ptr<Token> token;
	std::stack<double> stack;

	while(!expression.empty()) {
		token = expression.front();
		expression.pop();

		if (token->is_type("Number")) {
			Number n = *std::static_pointer_cast<Number>(token);
			stack.push(n);
		}
		else if (token->is_type("Function")) {
			std::shared_ptr<Function> f = std::static_pointer_cast<Function>(
				token
			);
			if (auto x = f->operator()(stack.top())) {
				stack.top() = *x;
			}
			else {
				return std::nullopt;
			}
		}
		else if (token->is_type("BasicFunction")) {
			auto f = *std::static_pointer_cast<BasicFunction>(token);
			stack.top() = f(stack.top());
		}
		else if (token->is_type("Operator")) {
			if (stack.size() < 2) {
				return std::nullopt;
			}
			Operator o = *std::static_pointer_cast<Operator>(token);
			double rhs = stack.top();
			stack.pop();
			double lhs = stack.top();
			stack.pop();
			stack.push(Operator::evaluate(o, lhs, rhs));
		}
		else {
			return std::nullopt;
		}
	}

	return stack.top();
}
std::optional<std::queue<std::shared_ptr<Token>>>
Calculator::shunting_yard(Expression input) {
	std::shared_ptr<Token> token;
	std::stack<std::shared_ptr<Token>> operator_stack;
	std::queue<std::shared_ptr<Token>> output_queue;

	for(auto const& token: input.expression) {
		if (token->is_type("Number")) {
			output_queue.push(token);
		}
		else if (token->is_type("Function") || token->is_type("BasicFunction")) {
			operator_stack.push(token);
		}
		else if (token->is_type("Operator")) {
			Operator o1 = *std::static_pointer_cast<Operator>(token);
			while (true) {
				if (operator_stack.empty()) break;
				if (operator_stack.top()->is_type("LeftParenthesis")) break; // note: tokens in operator_stack are always either Function, Operator or LeftBracket objects
				Operator o2 = *std::static_pointer_cast<Operator>(operator_stack.top());

				if (o2 > o1 || (o1 == o2 && o1.is_left_associative())) {
					output_queue.push(operator_stack.top());
					operator_stack.pop();
				}
				else {
					break;
				}
			}

			operator_stack.push(token);
		}
		else if (token->is_type("LeftParenthesis")) {
			operator_stack.push(token);
		}
		else if (token->is_type("RightParenthesis")) {
			while (!operator_stack.top()->is_type("LeftParenthesis")) { // todo: include error for mismatched parentheses (which would make operator_stack empty before a LeftParenthesis is found)
				output_queue.push(operator_stack.top());
				operator_stack.pop();
			}
			operator_stack.pop(); // get rid of the left parenthesis we just found

			if (
				!operator_stack.empty() && (
					operator_stack.top()->is_type("BasicFunction") ||
					operator_stack.top()->is_type("Function")
				)
			) {
				output_queue.push(operator_stack.top());
				operator_stack.pop();
			}
		}
		else {
			return std::nullopt;
		}
	}

	while (!operator_stack.empty()) {
		if(operator_stack.top()->is_type("LeftParenthesis")) {
			return std::nullopt;
		}

		output_queue.push(operator_stack.top());
		operator_stack.pop();
	}

	return output_queue;
}