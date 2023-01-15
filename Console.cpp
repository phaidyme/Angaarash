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
		else if (auto expression = parse<Expression>(command_line)) {
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

std::optional<Number> Console::evaluate(Expression expression) {
	Calculator& calculator = Calculator::get_instance();
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
	Calculator& calculator = Calculator::get_instance();

	auto let = 4;
	auto eqi = command.find('=');
	auto lbi = command.find('(');
	auto rbi = command.find(')');
	auto end = command.length();

	if (eqi == std::string::npos) {
		AddLog(
			"[error] invalid second argument for command 'let' (expected '=')\n"
		);
		return;
	}

	if (lbi < eqi) {
		if ( !( lbi<rbi && rbi<eqi )) {
			AddLog("[error] mismatched parenthesis\n");
			return;
		}

		std::string function_name = command.substr(let, lbi-let);
		trim_whitespace(function_name);
		if (!is_alpha(function_name)) {
			AddLog(
				"[error]: symbol \"%s\" is not a valid function name\n",
				function_name.c_str()
			);
			return;
		}
		if (calculator.variables.contains(function_name)) {
			AddLog(
				"[error] symbol '%s' already defined as equal to '%s'\n",
				function_name.c_str(),
				((std::string)calculator.variables.at(function_name)).c_str()
			);
			return;
		}
		if (calculator.functions.contains(function_name)) {
			AddLog(
				"[error] symbol '%s' already defined as a function\n",
				function_name.c_str()
			);
			return;
		}

		std::string argument_name = command.substr(1+lbi, rbi-(1+lbi));
		trim_whitespace(argument_name);
		if (!is_alpha(argument_name)) {
			AddLog(
				"[error]: symbol \"%s\" is not a valid argument name\n",
				argument_name.c_str()
			);
			return;
		}
		if (calculator.functions.contains(argument_name)) {
			AddLog(
				"[error] symbol '%s' already defined as a function\n",
				argument_name.c_str()
			);
			return;
		}

		std::string expression = command.substr(1+eqi, end-(1+eqi));
		calculator.arguments.insert(argument_name);
		if (auto parsed_expression = parse<Expression>(expression)) {
			calculator.functions.emplace(
				function_name, Function(
					function_name,
					argument_name,
					*parsed_expression)
			);
		}
		else {
			AddLog("[error]: failed to parse function expression \"%s\"\n",
			expression.c_str());
		}
		calculator.arguments.clear();
		return;
	}

	std::string var_name = command.substr(let, eqi-let);
	trim_whitespace(var_name);
	if (!is_alpha(var_name)) {
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
		if (auto expression = parse<Expression>(command)) {
			if (auto value = evaluate(*expression)) {
				calculator.variables.insert({var_name, *value});
			}
		}
	}
}
