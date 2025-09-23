#ifndef UTILS_HPP
# define UTILS_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <string>
#include <sstream>
#include <cctype>
#include <ctime>

// ------------------------------------------------
// functions
// ------------------------------------------------

std::string toLowerCase(const std::string& input);
std::string toString(time_t value);
std::string	toString(int num);

#endif
