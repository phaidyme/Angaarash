#include <ctype.h>

#include <iostream>

#include "Console.hpp"

void Console::ExecCommand(std::string const& command_line) {
		AddLog("# %s\n", command_line.c_str());

		// Insert into history. First find match and delete it so it can be pushed to the back.
		// This isn't trying to be smart or optimal.
		HistoryPos = -1;
		for (int i = history.size() - 1; i >= 0; i--) {
			if (stricmp(history[i], command_line) == 0) {
				history.erase(history.begin() + i);
				break;
			}
		}
		history.push_back(command_line);

		// Process command
		if (stricmp(command_line, "CLEAR") == 0) {
			items.clear();
		}
		else if (stricmp(command_line, "HELP") == 0) {
			AddLog("Commands:");
			for(auto const& command: commands) {
				AddLog("- %s", command.c_str());
			}
		}
		else if (stricmp(command_line, "HISTORY") == 0) {
			uint i = std::max(0, (int)history.size() - 10);
			for (; i < history.size(); i++) {
				AddLog("%3d: %s\n", i, history[i].c_str());
			}
		}
		else if (auto expression = parse_expression(command_line)) {
			if (auto postfix_expression = shunting_yard(*expression)) {
				if (auto value =
					evaluate_postfix_expression(*postfix_expression)) {
					AddLog("%s\n", std::string(*value).c_str());
				}
				else {
					AddLog("[error] expression is mathematically invalid");
				}
			}
			else {
				AddLog("[error] expression is syntactically invalid");
			}
		}
		else {
			AddLog("[error] Unknown command: '%s'\n", command_line.c_str());
		}

		// On command input, we scroll to bottom even if AutoScroll==false
		ScrollToBottom = true;
	}

void Console::Draw(const char* title, bool* p_open) {
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f,1.0f,1.0f,1.0f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
	if (!ImGui::Begin(title, p_open, ImGuiWindowFlags_NoTitleBar)) {
		ImGui::End();
		return;
	}


	// As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar.
	// So e.g. IsItemHovered() will return true when hovering the title bar.
	// Here we create a context menu only available from the title bar.
	if (ImGui::BeginPopupContextItem()) {
		if (ImGui::MenuItem("Close Console")) {
			*p_open = false;
		}
		ImGui::EndPopup();
	}

	// Options menu
	ImGui::Checkbox("Auto-scroll", &AutoScroll);
	ImGui::SameLine();
	ImGui::Dummy(ImVec2(ImGui::GetIO().DisplaySize.x - 400, 0));
	ImGui::SameLine();
	Filter.Draw("Filter", 180);
	ImGui::Separator();

	const float footer_height = ImGui::GetStyle().ItemSpacing.y +
								ImGui::GetFrameHeightWithSpacing();
	if (ImGui::BeginChild(
			"ScrollingRegion",
			ImVec2(0, -footer_height),
			false,
			ImGuiWindowFlags_HorizontalScrollbar)
		) {
		if (ImGui::BeginPopupContextWindow()) {
			if (ImGui::Selectable("Clear")) {
				items.clear();
			}
			ImGui::EndPopup();
		}

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
		
		for (uint i = 0; i < items.size(); i++) {
			std::string const& item = items[i];
			if (!Filter.PassFilter(item.c_str())) {
				continue;
			}
			if (strstr(item.c_str(), "[error]")) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
			}
			else if (strncmp(item.c_str(), "# ", 2) == 0) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.6f, 1.0f));
			}
			else {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
			}
			ImGui::TextUnformatted(item.c_str());
			ImGui::PopStyleColor();
		}

		// Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
		// Using a scrollbar or mouse-wheel will take away from the bottom edge.
		if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
			ImGui::SetScrollHereY(1.0f);
		ScrollToBottom = false;

		ImGui::PopStyleVar();
	}
	ImGui::EndChild();
	ImGui::Separator();

	// Command-line
	bool reclaim_focus = false;
	ImGuiInputTextFlags input_text_flags =
		ImGuiInputTextFlags_EnterReturnsTrue |
		ImGuiInputTextFlags_EscapeClearsAll |
		ImGuiInputTextFlags_CallbackCompletion |
		ImGuiInputTextFlags_CallbackHistory;
	if (ImGui::InputText(
			"Input",
			InputBuf,
			IM_ARRAYSIZE(InputBuf),
			input_text_flags,
			&TextEditCallbackStub,
			(void*)this)
	) {
		std::string s = InputBuf;
		strtrim(s);
		if (!s.empty()) {
			ExecCommand(s);
		}
		s = "";
		reclaim_focus = true;
	}

	// Auto-focus on window apparition
	ImGui::SetItemDefaultFocus();
	if (reclaim_focus) {
		ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
	}

	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
}

std::optional<Number>
evaluate_postfix_expression(std::queue<std::shared_ptr<Token>> expression) {
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
			Function f = *std::static_pointer_cast<Function>(token);
			stack.top() = f(stack.top());
		}
		else if (token->is_type("Operator")) {
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
shunting_yard(std::vector<std::shared_ptr<Token>> input) {
	
	std::shared_ptr<Token> token;
	std::stack<std::shared_ptr<Token>> operator_stack;
	std::queue<std::shared_ptr<Token>> output_queue;

	for(auto const& token: input) {
		if (token->is_type("Number")) {
			output_queue.push(token);
		}
		else if (token->is_type("Function")) {
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

			if (!operator_stack.empty() && operator_stack.top()->is_type("Function")) {
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

std::optional<std::vector<std::shared_ptr<Token>>>
parse_expression(std::string const& expression) {
	// get rid of whitespace (and const)
	std::string s;
	for(char c: expression) {
		if (!isspace(c)) {
			s.push_back(c);
		}
	}

	std::optional<std::shared_ptr<Token>> token;
	std::vector<std::shared_ptr<Token>> retval;
	while(!s.empty()) {
		token = read_token(s);
		if (token.has_value()) {
			retval.push_back(token.value());
		}
		else {
			return std::nullopt;
		}
	}

	return retval;
}
std::optional<std::shared_ptr<Token>> read_token(std::string & input) {
	assert(!input.empty());
	
	std::shared_ptr<Token> retval;

	if (auto x = Number::parse(input)) {
		retval = std::make_shared<Number>(*x);
	}
	else if (auto f = Function::parse(input)) {
		retval = std::make_shared<Function>(*f);
	}
	else if (Operator::is_operator(input[0])) {
		retval = std::make_shared<Operator>(input[0]);
		input.erase(0, 1);
	}
	else if (input[0] == '(') {
		retval = std::make_shared<LeftParenthesis>();
		input.erase(0, 1);
	}
	else if (input[0] == ')') {
		retval = std::make_shared<RightParenthesis>();
		input.erase(0, 1);
	}
	else {
		return std::nullopt;
	}

	return retval;
}
