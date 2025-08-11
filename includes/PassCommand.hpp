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
		Command* createPassCommand() const;

		const t_response	execute(const t_parserd& input) const;
};

// ------------------------------------------------
// function
// ------------------------------------------------

#endif
