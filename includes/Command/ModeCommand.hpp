#ifndef MODECOMMAND_HPP
# define MODECOMMAND_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>
#include <cctype>
#include "Command.hpp"

// ------------------------------------------------
// class
// ------------------------------------------------

class ModeCommand : public Command
{

public:

	ModeCommand();
	~ModeCommand();

	static Command*			createModeCommand();
	std::vector<t_response>	execute(const t_parsed & input, Database & db) const;

private:

	bool	isValidCmd(const t_parsed & input, t_response & res, Client & client, Database & db) const;

	void	buildChannelModeReply(Channel & ch, std::string & modes, std::vector<std::string> & params);
	bool	isKnownMode(char c);
	bool	needsParameter(char c, char sign);

};

// ------------------------------------------------
// function
// ------------------------------------------------


#endif
