#ifndef CAPCOMMAND_HPP
# define CAPCOMMAND_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>
#include "Command.hpp"

// ------------------------------------------------
// class
// ------------------------------------------------

class CapCommand : public Command
{

public:

	CapCommand();
	~CapCommand();

	static Command*			createCapCommand();
	std::vector<t_response>	execute(const t_parsed & input, Database & db) const;

private:

	bool	isValidCmd(const t_parsed & input, t_response & res) const;

};

// ------------------------------------------------
// function
// ------------------------------------------------

#endif
