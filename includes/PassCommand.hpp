#ifndef PASSCOMMAND_HPP
# define PASSCOMMAND_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>
#include "Command.hpp"
#include "Database.hpp"

// ------------------------------------------------
// class
// ------------------------------------------------

class PassCommand : public Command {
	public:
		PassCommand();
		~PassCommand();

		const t_response	execute(const t_parsed& input) const;
};

// ------------------------------------------------
// function
// ------------------------------------------------

#endif
