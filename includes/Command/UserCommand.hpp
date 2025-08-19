#ifndef USERCOMMAND_HPP
# define USERCOMMAND_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>
#include "Command.hpp"

// ------------------------------------------------
// class
// ------------------------------------------------

class UserCommand : public Command
{

public:

	UserCommand();
	~UserCommand();

	static Command *	createUserCommand(void);
	const t_response	execute(const t_parsed& input, Database& db) const;

private:

	bool	isValidCmd(const t_parsed & input, t_response * res, Client * client) const;

};

// ------------------------------------------------
// function
// ------------------------------------------------

#endif
