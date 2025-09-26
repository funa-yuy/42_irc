#ifndef QUITCOMMAND_HPP
# define QUITCOMMAND_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>
#include "Command.hpp"

// ------------------------------------------------
// class
// ------------------------------------------------

class QuitCommand : public Command
{

public:

	QuitCommand();
	~QuitCommand();

	static Command*			createQuitCommand();
	std::vector<t_response>	execute(const t_parsed & input, Database & db) const;

private:

	bool	isValidCmd(const t_parsed & input, t_response & res, Client & client) const;

};

// ------------------------------------------------
// function
// ------------------------------------------------


#endif
