#include <ctype.h>

#include <iostream>
#include <algorithm>

#include "Console.hpp"

void Console::ExecCommand(std::string& command_line) {
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
		else if (command_line.find("let ") == 0) {
			let(command_line);
		}
		else if (auto expression = parse_expression(command_line)) {
			evaluate(*expression);
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

std::optional<Number> Console::evaluate(std::vector<std::shared_ptr<Token>> expression) {
	std::optional<Number> x = calculator.evaluate(expression);
	if (x.has_value()) {
		AddLog("%s\n", std::string(*x).c_str());
	}
	else {
		AddLog("[error] invalid mathematical expression");
	}
	return x;
}
void Console::let(std::string& command) {
	assert(command.find("let ") == 0);
	command.erase(0, 4);
	if (command.find('=') == std::string::npos) {
		AddLog(
			"[error] invalid second argument for command 'let' (expected '=')"
		);
	}
	else {
		std::string var_name = command.substr(0, command.find('='));
		var_name.erase(
			remove(var_name.begin(), var_name.end(), ' '), var_name.end()
		);
		bool is_alpha = true;
		for(char c: var_name) {
			if (!isalpha(c)) {
				is_alpha = false;
			}
		}
		if (!is_alpha) {
			AddLog(
				"[error] symbol '%s' is not a valid variable name\n",
				var_name.c_str()
			);
		}
		else if (calculator.variables.contains(var_name)) {
			AddLog(
				"[error] symbol '%s' already defined as equal to '%s'\n",
				var_name.c_str(),
				((std::string)calculator.variables.at(var_name)).c_str()
			);
		}
		else if (calculator.functions.contains(var_name)) {
			AddLog(
				"[error] symbol '%s' already defined as a function\n",
				var_name.c_str()
			);
		}
		else {
			command.erase(0, 1+command.find('='));
			if (auto expression = parse_expression(command)) {
				if (auto value = evaluate(*expression)) {
					calculator.variables.insert({var_name, *value});
				}
			}
		}
	}
}
std::optional<std::vector<std::shared_ptr<Token>>>
Console::parse_expression(std::string const& expression) {
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
std::optional<std::shared_ptr<Token>> Console::read_token(std::string & input) {
	assert(!input.empty());
	
	std::shared_ptr<Token> retval;

	if (auto x = Number::parse(input)) {
		retval = std::make_shared<Number>(*x);
	}
	else if (auto x = read_variable(input)) {
		retval = std::make_shared<Variable>(*x);
	}
	else if (auto f = read_function(input)) {
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
std::optional<Variable> Console::read_variable(std::string & input) {
	for(const auto& x: calculator.variables) {
		if(input.find(x.first) == 0) {
			input.erase(0, x.first.length());
			return Variable(x.first, x.second);
		}
	}
	return std::nullopt;
}
std::optional<Function> Console::read_function(std::string & input) {
	for(auto const& f: calculator.functions) {
		if (input.find(f.first) == 0) {
			input.erase(0, f.first.length());
			return Function(f.first, f.second);
		}
	}
	return std::nullopt;
}