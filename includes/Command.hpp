#ifndef COMMAND_HPP
# define COMMAND_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>
#include "irc.hpp"

// ------------------------------------------------
// class
// ------------------------------------------------

class Command {
	public:
		Command();
		virtual ~Command() = 0;

		virtual const t_response	execute(const t_parsed& input, Database db) const = 0;
};

// ------------------------------------------------
// function
// ------------------------------------------------


#endif
