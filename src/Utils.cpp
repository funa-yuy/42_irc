#include "Utils.hpp"
#include <cstdio>
#include <sstream>

std::string toLowerCase(const std::string& input)
{
	std::string lowered = input;
	for (size_t i = 0; i < lowered.size(); ++i)
		lowered[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(lowered[i])));
	return (lowered);
}

std::string toString(time_t value)
{
	std::ostringstream oss;
	oss << static_cast<unsigned long>(value);
	return oss.str();
}
