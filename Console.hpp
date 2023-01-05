#include <cstdlib>
#include <cstring>

#include <vector>
#include <queue>
#include <stack>
#include <memory>
#include <optional>

#include "imgui/imgui.h"

#include "Token.hpp"
#include "Calculator.hpp"
#include "helper_functions.hpp"

class Console {
	char InputBuf[256];
	std::vector<std::string> items;
	std::vector<std::string> commands;
	std::vector<std::string> history;
	int HistoryPos;	// -1: new line, 0..History.Size-1 browsing history.
	ImGuiTextFilter Filter;
	bool AutoScroll, ScrollToBottom;

	Calculator calculator;


	std::optional<std::vector<std::shared_ptr<Token>>>
		parse_expression(std::string const&);
	std::optional<std::shared_ptr<Token>> read_token(std::string &);
	std::optional<Variable> read_variable(std::string &);
	std::optional<Function> read_function(std::string &);
public:
	Console(): HistoryPos(-1), AutoScroll(true), ScrollToBottom(false) {
		memset(InputBuf, 0, sizeof(InputBuf));

		commands.push_back("HELP");
		commands.push_back("HISTORY");
		commands.push_back("CLEAR");
		commands.push_back("CLASSIFY");
	}

	void AddLog(const char* fmt, ...) IM_FMTARGS(2) {
		char buf[1024];
		va_list args;
		va_start(args, fmt);
		vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
		buf[IM_ARRAYSIZE(buf)-1] = 0;
		va_end(args);
		items.push_back(std::string(buf));
	}

	void Draw(const char*, bool*);
	void ExecCommand(std::string&);

	static int TextEditCallbackStub(ImGuiInputTextCallbackData* data) {
		Console* console = (Console*)data->UserData;
		return console->TextEditCallback(data);
	}

	int TextEditCallback(ImGuiInputTextCallbackData* data) {
		//AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
		switch (data->EventFlag) {
		case ImGuiInputTextFlags_CallbackCompletion:
		{
			// Example of TEXT COMPLETION

			// Locate beginning of current word
			const char* word_end = data->Buf + data->CursorPos;
			const char* word_start = word_end;
			while (word_start > data->Buf) {
				const char c = word_start[-1];
				if (c == ' ' || c == '\t' || c == ',' || c == ';') {
					break;
				}
				word_start--;
			}

			// Build a list of candidates
			std::vector<std::string> candidates;
			for (uint i = 0; i < commands.size(); i++)
				if (strnicmp(commands[i], word_start, (int)(word_end - word_start)) == 0)
					candidates.push_back(commands[i]);

			if (candidates.size() == 0) {
				AddLog("No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
			}
			else if (candidates.size() == 1) {
				// Single match. Delete the beginning of the word and replace it entirely so we've got nice casing.
				data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
				data->InsertChars(data->CursorPos, candidates[0].c_str());
				data->InsertChars(data->CursorPos, " ");
			}
			else {
				// Multiple matches. Complete as much as we can..
				// So inputing "C"+Tab will complete to "CL" then display "CLEAR" and "CLASSIFY" as matches.
				int match_len = (int)(word_end - word_start);
				while(true) {
					int c = 0;
					bool all_candidates_matches = true;
					for (uint i = 0; i < candidates.size() && all_candidates_matches; i++) {
						if (i == 0) {
							c = toupper(candidates[i][match_len]);
						}
						else if (c == 0 || c != toupper(candidates[i][match_len])) {
							all_candidates_matches = false;
						}
					}
					if (!all_candidates_matches) {
						break;
					}
					match_len++;
				}

				if (match_len > 0) {
					data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
					data->InsertChars(data->CursorPos, candidates[0].data(), candidates[0].data() + match_len);
				}

				// List matches
				AddLog("Possible matches:\n");
				for (uint i = 0; i < candidates.size(); i++) {
					AddLog("- %s\n", candidates[i].c_str());
				}
			}

			break;
		}
		case ImGuiInputTextFlags_CallbackHistory: {
			// Example of HISTORY
			const int prev_history_pos = HistoryPos;
			if (data->EventKey == ImGuiKey_UpArrow) {
				if (HistoryPos == -1) {
					HistoryPos = history.size() - 1;
				}
				else if (HistoryPos > 0) {
					HistoryPos--;
				}
			}
			else if (data->EventKey == ImGuiKey_DownArrow) {
				if (HistoryPos != -1) {
					if (++HistoryPos >= (int)history.size()) {
						HistoryPos = -1;
					}
				}
			}

			// A better implementation would preserve the data on the current input line along with cursor position.
			if (prev_history_pos != HistoryPos) {
				const std::string history_str = (HistoryPos >= 0) ? history[HistoryPos] : "";
				data->DeleteChars(0, data->BufTextLen);
				data->InsertChars(0, history_str.c_str());
			}
		}
		}
		return 0;
	}
};