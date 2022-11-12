#include "evaluator.h"

double evaluate_postfix_expression(std::queue<std::shared_ptr<Token>> expression) {
	std::shared_ptr<Token> token;
	std::stack<double> stack;

	while (! expression.empty()) {
		token = expression.front();
		expression.pop();

		if (token->is_type("Number")) {
			Number n = *static_pointer_cast<Number>(token);
			stack.push(n);
		}
		else if (token->is_type("Function")) {
			Function f = *static_pointer_cast<Function>(token);
			stack.top() = Function::evaluate(f, stack.top());
		}
		else if (token->is_type("Operator")) {
			Operator o = *static_pointer_cast<Operator>(token);
			double rhs = stack.top();
			stack.pop();
			double lhs = stack.top();
			stack.pop();
			stack.push(Operator::evaluate(o, lhs, rhs));
		}
		else {
			throw std::runtime_error("invalid token in postfix expression");
		}
	}

	return stack.top();
}
		
std::queue<std::shared_ptr<Token>> shunting_yard(String input) {
	std::shared_ptr<Token> token;
	std::stack<std::shared_ptr<Token>> operator_stack;
	std::queue<std::shared_ptr<Token>> output_queue;

	while(!input.is_empty()) {
		token = read_token(input);

		if (token->is_type("Number")) {
			output_queue.push(token);
		}
		else if (token->is_type("Function")) {
			operator_stack.push(token);
		}
		else if (token->is_type("Operator")) {
			Operator o1 = *static_pointer_cast<Operator>(token);
			while (true) {
				if (operator_stack.empty()) break;
				if (operator_stack.top()->is_type("LeftParenthesis")) break; // note: tokens in operator_stack are always either Function, Operator or LeftBracket objects
				Operator o2 = *static_pointer_cast<Operator>(operator_stack.top());

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

			if (!operator_stack.empty() && operator_stack.top()->is_type("Function")) {
				output_queue.push(operator_stack.top());
				operator_stack.pop();
			}
		}
		else {
			throw std::runtime_error("invalid Token");
		}
	}

	while (!operator_stack.empty()) {
		assert(! operator_stack.top()->is_type("LeftParenthesis"));

		output_queue.push(operator_stack.top());
		operator_stack.pop();
	}

	return output_queue;
}

std::shared_ptr<Token> read_token(String & input) {
	assert(!input.is_empty());
	
	std::shared_ptr<Token> retval;

	if (isdigit(input[0])) {
		retval = std::make_shared<Number>(read_number(input));
	}
	else if (Operator::is_operator(input[0])) {
		retval = std::make_shared<Operator>(input[0]);
		input = input.slice(1, input.get_len());
	}
	else if (input[0] == '(') {
		retval = std::make_shared<LeftParenthesis>();
		input = input.slice(1, input.get_len());
	}
	else if (input[0] == ')') {
		retval = std::make_shared<RightParenthesis>();
		input = input.slice(1, input.get_len());
	}
	else {
		throw std::runtime_error("unrecognized token in command content");
	}

	return retval;
}

double read_number(String & input) {
	assert(isdigit(input[0]));

	// count digits + possible decimal point
	int n = 0;
	bool passed_decimal = false;
	while (n < input.get_len()) {
		if (isdigit(input[n])) {
			n++;
		}
		else {
			if (passed_decimal) {
				break;
			}
			else {
				if (input[n] == '.') {
					passed_decimal = true;
					n++;
				}
				else {
					break;
				}
			}
		}
	}
	// make input look like a c string before passing it to atof() to do the actual number reading
	input.push('\0');
	double number_val = atof(input.begin());
	input.pop();

	input = input.slice(n, input.get_len());

	return number_val;
}