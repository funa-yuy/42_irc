#ifndef PINGCOMMAND_HPP
# define PINGCOMMAND_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>
#include "Command.hpp"

// ------------------------------------------------
// class
// ------------------------------------------------

class PingCommand : public Command
{

public:

	PingCommand();
	~PingCommand();

	static Command*			createPingCommand();
	std::vector<t_response>	execute(const t_parsed & input, Database & db) const;

private:

	bool	isValidCmd(const t_parsed & input, t_response & res) const;

};

// ------------------------------------------------
// function
// ------------------------------------------------


#endif
