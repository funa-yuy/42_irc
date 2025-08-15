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

		const t_response	execute(const t_parsed& input, Database& db) const;
};

// ------------------------------------------------
// function
// ------------------------------------------------

#endif
