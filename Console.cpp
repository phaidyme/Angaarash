#include <ctype.h>

#include <iostream>
#include <algorithm>

#include "Console.hpp"

Console::Console(): HistoryPos(-1) {
	commands.push_back("HELP");
	commands.push_back("HISTORY");
	commands.push_back("CLEAR");
	commands.push_back("CLASSIFY");
	commands.push_back("LET");
}

void Console::process_command(std::string& command_line) {
	if (command_line.length() == 0) {
		return;
	}

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
		AddLog("[error] Unknown command");
	}
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

void Console::render() {
	ImVec2 total = ImGui::GetIO().DisplaySize;

	auto x1 = total.x * 0.7;
	auto x2 = total.x - x1;

	auto y2 = ImGui::GetStyle().ItemSpacing.y + 10 +
		ImGui::GetFrameHeightWithSpacing();
	auto y1 = total.y - y2;

	render_console(ImVec2(0,0), ImVec2(x1,y1));
	render_memory(ImVec2(x1,0), ImVec2(x2,y1));
	render_prompt(ImVec2(0,y1), ImVec2(total.x,y2));
}

void Console::render_console(ImVec2 position, ImVec2 size) {
	ImGui::SetNextWindowSize(size);
	ImGui::SetNextWindowPos(position);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f,1.0f,1.0f,1.0f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
	if (!ImGui::Begin("console", nullptr, ImGuiWindowFlags_NoTitleBar)) {
		ImGui::End();
		return;
	}

	// Tighten spacing
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));
	auto error_colour = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
	auto command_colour = ImVec4(0.7f, 0.56f, 0.42f, 1.0f);
	auto response_colour = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	
	for (uint i = 0; i < items.size(); i++) {
		std::string const& item = items[i];
		if (strstr(item.c_str(), "[error]")) {
			ImGui::PushStyleColor(ImGuiCol_Text, error_colour);
		}
		else if (strncmp(item.c_str(), "# ", 2) == 0) {
			ImGui::PushStyleColor(ImGuiCol_Text, command_colour);
		}
		else {
			ImGui::PushStyleColor(ImGuiCol_Text, response_colour);
		}
		ImGui::TextUnformatted(item.c_str());
		ImGui::PopStyleColor();
	}

	if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
		ImGui::SetScrollHereY(1.0f);
	}

	ImGui::PopStyleVar();
	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
}

void Console::render_memory(ImVec2 position, ImVec2 size) {
	ImGui::SetNextWindowSize(size);
	ImGui::SetNextWindowPos(position);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f,1.0f,1.0f,1.0f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
	if (!ImGui::Begin("memory", nullptr, ImGuiWindowFlags_NoTitleBar)) {
		ImGui::End();
		return;
	}
	
	auto& calculator = Calculator::get_instance();
	if (ImGui::BeginChild("Functions", ImVec2(0, size.y / 2))) {
		for(const auto& function: calculator.functions) {
			auto f_sig = function.second.get_signature();
			auto f_exp = function.second.get_expression();
			auto f_all = f_sig + " = " + f_exp;
			ImGui::TextUnformatted(f_all.c_str());
		}
	}
	ImGui::EndChild();
	if (ImGui::BeginChild("Variables", ImVec2(0, size.y / 2))) {
		for(const auto& variable: calculator.variables) {
			std::string tmp = variable.first + " = " + (std::string)variable.second;
			ImGui::TextUnformatted(tmp.c_str());
		}
	}
	ImGui::EndChild();

	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
}

void Console::render_prompt(ImVec2 position, ImVec2 size) {
	ImGui::SetNextWindowSize(size);
	ImGui::SetNextWindowPos(position);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f,1.0f,1.0f,1.0f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
	if (!ImGui::Begin("prompt", nullptr, ImGuiWindowFlags_NoTitleBar)) {
		ImGui::End();
		return;
	}
	// Command-line
	bool reclaim_focus = false;
	std::size_t buffer_size = 256;
	char buffer[buffer_size];
	memset(buffer, 0, buffer_size);
	ImGuiInputTextFlags input_text_flags =
		ImGuiInputTextFlags_EnterReturnsTrue |
		ImGuiInputTextFlags_EscapeClearsAll |
		ImGuiInputTextFlags_CallbackCompletion |
		ImGuiInputTextFlags_CallbackHistory;
	if (ImGui::InputText(
			"Input",
			buffer,
			buffer_size,
			input_text_flags,
			&TextEditCallbackStub,
			(void*)this)
	) {
		std::string input = buffer;
		process_command(input);
		ImGui::SetScrollHereY(1.0f);
		reclaim_focus = true;
	}

	// Auto-focus on window apparition
	ImGui::SetItemDefaultFocus();
	if (reclaim_focus) {
		ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
	}

	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::End();
}