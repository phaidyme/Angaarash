#include <cstdlib>
#include <cstring>

#include <vector>

#include "imgui/imgui.h"

#include "helper_functions.hpp"

class Console {
	char InputBuf[256];
	std::vector<std::string> items;
	std::vector<std::string> commands;
	std::vector<std::string> history;
	int HistoryPos;	// -1: new line, 0..History.Size-1 browsing history.
	ImGuiTextFilter Filter;
	bool AutoScroll, ScrollToBottom;

public:
	Console(): HistoryPos(1), AutoScroll(true), ScrollToBottom(false) {
		memset(InputBuf, 0, sizeof(InputBuf));

		commands.push_back("HELP");
		commands.push_back("HISTORY");
		commands.push_back("CLEAR");
		commands.push_back("CLASSIFY");
		
		AddLog("Welcome to Dear ImGui!");
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

	void Draw(const char* title, bool* p_open) {
		ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin(title, p_open)) {
			ImGui::End();
			return;
		}

		// As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar.
		// So e.g. IsItemHovered() will return true when hovering the title bar.
		// Here we create a context menu only available from the title bar.
		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Close Console"))
				*p_open = false;
			ImGui::EndPopup();
		}

		ImGui::TextWrapped(
			"This example implements a console with basic coloring, completion (TAB key) and history (Up/Down keys). A more elaborate "
			"implementation may want to store entries along with extra data such as timestamp, emitter, etc.");
		ImGui::TextWrapped("Enter 'HELP' for help.");

		ImGui::Separator();

		// Options menu
		if (ImGui::BeginPopup("Options"))
		{
			ImGui::Checkbox("Auto-scroll", &AutoScroll);
			ImGui::EndPopup();
		}

		// Options, Filter
		if (ImGui::Button("Options"))
			ImGui::OpenPopup("Options");
		ImGui::SameLine();
		Filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
		ImGui::Separator();

		// Reserve enough left-over height for 1 separator + 1 input text
		const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
		if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar))
		{
			if (ImGui::BeginPopupContextWindow())
			{
				if (ImGui::Selectable("Clear")) {
					items.clear();
				}
				ImGui::EndPopup();
			}

			// Display every line as a separate entry so we can change their color or add custom widgets.
			// If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
			// NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping
			// to only process visible items. The clipper will automatically measure the height of your first item and then
			// "seek" to display only items in the visible area.
			// To use the clipper we can replace your standard loop:
			//	  for (int i = 0; i < Items.Size; i++)
			//   With:
			//	  ImGuiListClipper clipper;
			//	  clipper.Begin(Items.Size);
			//	  while (clipper.Step())
			//		 for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
			// - That your items are evenly spaced (same height)
			// - That you have cheap random access to your elements (you can access them given their index,
			//   without processing all the ones before)
			// You cannot this code as-is if a filter is active because it breaks the 'cheap random-access' property.
			// We would need random-access on the post-filtered list.
			// A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices
			// or offsets of items that passed the filtering test, recomputing this array when user changes the filter,
			// and appending newly elements as they are inserted. This is left as a task to the user until we can manage
			// to improve this example code!
			// If your items are of variable height:
			// - Split them into same height items would be simpler and facilitate random-seeking into your list.
			// - Consider using manual call to IsRectVisible() and skipping extraneous decoration from your items.
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
			
			for (int i = 0; i < items.size(); i++) {
				std::string const& item = items[i];
				if (!Filter.PassFilter(item.c_str())) {
					continue;
				}
				ImVec4 color;
				bool has_color = false;
				if (strstr(item.c_str(), "[error]")) {
					color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
					has_color = true;
				}
				else if (strncmp(item.c_str(), "# ", 2) == 0) {
					color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f);
					has_color = true;
				}
				if (has_color) {
					ImGui::PushStyleColor(ImGuiCol_Text, color);
				}
				ImGui::TextUnformatted(item.c_str());
				if (has_color) {
					ImGui::PopStyleColor();
				}
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
		ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
		if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), input_text_flags, &TextEditCallbackStub, (void*)this)) {
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
		if (reclaim_focus)
			ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

		ImGui::End();
	}

	void ExecCommand(std::string const& command_line) {
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
			int i = std::max(0, (int)history.size() - 10);
			for (; i < history.size(); i++) {
				AddLog("%3d: %s\n", i, history[i].c_str());
			}
		}
		else {
			AddLog("Unknown command: '%s'\n", command_line.c_str());
		}

		// On command input, we scroll to bottom even if AutoScroll==false
		ScrollToBottom = true;
	}

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
				if (c == ' ' || c == '\t' || c == ',' || c == ';')
					break;
				word_start--;
			}

			// Build a list of candidates
			std::vector<std::string> candidates;
			for (int i = 0; i < commands.size(); i++)
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
					for (int i = 0; i < candidates.size() && all_candidates_matches; i++) {
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
				for (int i = 0; i < candidates.size(); i++) {
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
					if (++HistoryPos >= history.size()) {
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