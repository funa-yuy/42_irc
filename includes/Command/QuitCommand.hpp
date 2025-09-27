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

	t_response	makeBroadcastQuit(const std::string & msg, Client & sender, Database & db) const;
	t_response	makeRplError(const std::string & msg, Client & sender) const;

};

// ------------------------------------------------
// function
// ------------------------------------------------


#endif
