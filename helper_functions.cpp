#include <cassert>

#include <string>

#include "helper_functions.hpp"

int strnicmp(std::string const& s1, std::string const& s2, std::size_t n) {
	std::size_t i=0;
	int difference = toupper(s1[i]) - toupper(s2[i]);

	while(i<n && difference == 0 && i < s1.length() && i < s2.length()) {
		i++;
		difference = toupper(s1[i]) - toupper(s2[i]);
	}

	return difference;
}
int stricmp(std::string const& s1, std::string const& s2) {
	return strnicmp(s1, s2, s1.length());
}
void strtrim(std::string & s) {
	while(s.back() == ' ') {
		s.pop_back();
	}
}
// both ends
void trim_whitespace(std::string& s) {
	int begin, end;
	for(begin = 0; std::isspace(s[begin]); begin++) {}
	for(end = s.length(); std::isspace(s[end-1]); end--) {}
	
	s = s.substr(begin, end-begin);
}

bool is_alpha(const std::string& s) {
	for(char c: s) {
		if (!std::isalpha(c)) {
			return false;
		}
	}
	return true;
}

bool string_length_comp::operator()(std::string a, std::string b) const {
	if (a.length() == b.length()) {
		return a < b;
	}
	return a.length() > b.length();
}