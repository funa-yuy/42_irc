#ifndef JOINCOMMAND_HPP
# define JOINCOMMAND_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>
#include "Command.hpp"

// ------------------------------------------------
// class
// ------------------------------------------------

class JoinCommand : public Command {
	public:
		JoinCommand();
		~JoinCommand();
		static Command* createJoinCommand();

		const t_response	execute(const t_parsed& input, Database& db) const;
};

// ------------------------------------------------
// function
// ------------------------------------------------

#endif
