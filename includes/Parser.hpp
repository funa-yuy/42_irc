#ifndef PARSER_HPP
 #define PARSER_HPP

 #include <string>
 #include <iostream>
 #include <sstream>
 #include <vector>
 #include "irc.hpp"

 #define MAX_MSG_ARG 15

class Parser
{

public:

	static t_parsed exec(std::string line, int client_fd);

private:

	static void trimCRLF(std::string & s);
	static void	extractTrailing(std::string & s, std::string & trailing);
	static void	tokenize(std::string & s, std::vector<std::string> & tokens);
	static void	toUpperCase(std::string & s);

};

#endif
