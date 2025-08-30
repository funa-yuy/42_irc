#ifndef PRIVMSGCOMMAND_HPP
# define PRIVMSGCOMMAND_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>
#include "Command.hpp"
#include "Database.hpp"

#define EXCLAMATION_CHANNEL_MAX 6

// ------------------------------------------------
// class
// ------------------------------------------------

class PrivmsgCommand : public Command {
	public:
		PrivmsgCommand();
		~PrivmsgCommand();
		static Command* createPrivmsgCommand();

		const t_response	execute(const t_parsed& input, Database& db) const;
};

// ------------------------------------------------
// function
// ------------------------------------------------

#endif
