#include <cassert>

#include <string>

#include "helper_functions.hpp"

int strnicmp(std::string const& s1, std::string const& s2, uint n) {
	uint i=0;
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