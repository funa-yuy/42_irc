#ifndef PARSER_HPP
 #define PARSER_HPP

 #include <string>
 #include <iostream>
 #include <sstream>
 #include <vector>
 #include "irc.hpp"

 #define MAX_MSG_ARG 15

void print_debug(std::string msg);

class Parser
{
	public:
		static t_parsed exec(std::string line, int client_fd);
	private:
		std::string _line;
};

#endif
