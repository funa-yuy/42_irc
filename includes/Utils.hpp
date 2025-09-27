#ifndef UTILS_HPP
# define UTILS_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <string>
#include <cctype>
#include <ctime>
#include <cstdio>
#include <sstream>
#include <vector>

// ------------------------------------------------
// functions
// ------------------------------------------------

std::string toLowerCase(const std::string& input);
std::string toString(time_t value);
std::vector<std::string> split(const std::string& str, char delimiter);

#endif
