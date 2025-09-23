#include "Utils.hpp"

std::string toLowerCase(const std::string& input)
{
	std::string lowered = input;
	for (size_t i = 0; i < lowered.size(); ++i)
		lowered[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(lowered[i])));
	return (lowered);
}

std::string	toString(int num)
{
	std::ostringstream oss;
	oss << num;
	return (oss.str());
}
