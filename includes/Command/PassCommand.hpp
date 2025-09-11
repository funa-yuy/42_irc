#ifndef PASSCOMMAND_HPP
# define PASSCOMMAND_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>
#include "Command.hpp"

// ------------------------------------------------
// class
// ------------------------------------------------

class PassCommand : public Command {
	public:
		PassCommand();
		~PassCommand();
		static Command* createPassCommand();

		std::vector<t_response>	execute(const t_parsed& input, Database& db) const;
};

// ------------------------------------------------
// function
// ------------------------------------------------

#endif
