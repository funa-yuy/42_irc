#ifndef PONGCOMMAND_HPP
# define PONGCOMMAND_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>
#include "Command.hpp"

// ------------------------------------------------
// class
// ------------------------------------------------

class PongCommand : public Command
{

public:

	PongCommand();
	~PongCommand();

	static Command*					createPongCommand();
	const std::vector<t_response>	execute(const t_parsed& input, Database& db) const;

private:

	bool	isValidCmd(const t_parsed & input, t_response & res, Client & client) const;

};

// ------------------------------------------------
// function
// ------------------------------------------------


#endif
