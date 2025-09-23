#ifndef MODECOMMAND_HPP
# define MODECOMMAND_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "Channel.hpp"
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

	struct ModeOp
	{
		char		sign;
		char		mode;
		std::string	param;
		ModeOp(char s, char m, const std::string & p) : sign(s), mode(m), param(p) {}
	};

	bool	isValidCmd(const t_parsed & input, t_response & res, Client & client, Database & db) const;

	bool	checkViewPermissions(Channel & ch, const Client & client, int fd, t_response & res, const std::string & chName) const;
	bool	checkModifyPermissions(Channel & ch, const Client & client, int fd, t_response & res, const std::string & chName) const;
	bool	parseModesAndParams(const std::string & modeStr, const std::vector<std::string> & params,
								std::vector<ModeOp> & ops, t_response & res, const Client & client) const;
	bool	validateSemantic(const std::vector<ModeOp> & ops, Channel & ch, Database & db,
							const Client & client, const std::string & chName, t_response & res) const;

	void	buildChannelModeReply(const Channel & ch, std::string & modes, std::vector<std::string> & params) const;
	bool	isKnownMode(char c) const;
	bool	needsParameter(char c, char sign) const;

};

// ------------------------------------------------
// function
// ------------------------------------------------


#endif
