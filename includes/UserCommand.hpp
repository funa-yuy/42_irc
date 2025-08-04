#ifndef USERCOMMAND_HPP
# define USERCOMMAND_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>
#include "Command.hpp"
#include "Database.hpp"

// ------------------------------------------------
// class
// ------------------------------------------------

class UserCommand : public Command {
	public:
		UserCommand();
		~UserCommand();

		const t_response	execute(char* input) const;
};

// ------------------------------------------------
// function
// ------------------------------------------------

#endif
