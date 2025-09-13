#ifndef JOINCOMMAND_HPP
# define JOINCOMMAND_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>
#include "Command.hpp"

struct s_join_item {
	std::string	channel;
	std::string	key;
};

// ------------------------------------------------
// class
// ------------------------------------------------

class JoinCommand : public Command {
	public:
		JoinCommand();
		~JoinCommand();
		static Command* createJoinCommand();

		std::vector<t_response>	execute(const t_parsed& input, Database& db) const;
};

// ------------------------------------------------
// function
// ------------------------------------------------

#endif
