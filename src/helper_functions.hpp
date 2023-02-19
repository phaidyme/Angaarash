#pragma once

#include <vector>
#include <string>

#define print(x) std::cout << x
#define println(x) std::cout << x << std::endl;

std::vector<std::string> split(std::string const&, char);

int strnicmp(std::string const&, std::string const&, std::size_t);

int stricmp(std::string const&, std::string const&);

void strtrim(std::string &);

template <char c>
void trim_leading(std::string& s) {
	auto i = s.find_first_not_of(c);
	if (i != std::string::npos) {
		s = s.substr(i);
	}
}

void trim_whitespace(std::string&);

bool is_alpha(const std::string&);

// used to sort function/variable priority queues by length
class string_length_comp {
public:
	bool operator ()(std::string, std::string) const;
};
