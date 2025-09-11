#ifndef COMMAND_HPP
# define COMMAND_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>
#include "irc.hpp"
#include "Database.hpp"
#include <vector>

// ------------------------------------------------
// class
// ------------------------------------------------

class Command {
	public:
		Command();
		virtual ~Command() = 0;

		virtual std::vector<t_response>	execute(const t_parsed& input, Database& db) const = 0;
};

// ------------------------------------------------
// function
// ------------------------------------------------


#endif
