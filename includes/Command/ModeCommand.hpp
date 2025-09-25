#ifndef MODECOMMAND_HPP
# define MODECOMMAND_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>
#include <cctype>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>
#include "Command.hpp"
#include "Channel.hpp"
#include "Utils.hpp"

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

	std::vector<t_response>	handleModeView(Client & sender, Channel & ch) const;
	t_response	makeRplChannelModeIs(const Client & sender, const Channel & ch) const;
	void		buildChannelModeReply(const Channel & ch, std::string & modes, std::vector<std::string> & params) const;
	t_response	makeRplCreationTime(const Client & sender, const Channel & ch) const;

	std::vector<t_response>	handleModeChange(const t_parsed & input, Database & db, Client & sender, Channel & ch) const;
	bool	parseModesAndParams(const std::string & modeStr, const std::vector<std::string> & params, std::vector<ModeOp> & ops, t_response & res, const Client & client) const;
	bool	validateSemantic(const std::vector<ModeOp> & ops, Channel & ch, Database & db, const Client & client, const std::string & chName, t_response & res) const;
	bool	checkPermissions(Channel & ch, const Client & client, int fd, t_response & res, const std::string & chName) const;

	bool	isKnownMode(char c) const;
	bool	needsParameter(char c, char sign) const;

	bool	applyModes(Channel & ch, Database & db, const std::vector<ModeOp> & ops, std::string & outModes, std::vector<std::string> & outParams) const;

	bool	applyModeInviteOnly(Channel & ch, const ModeOp & op) const;
	bool	applyModeTopicRestricted(Channel & ch, const ModeOp & op) const;
	bool	applyModeKey(Channel & ch, const ModeOp & op) const;
	bool	applyModeLimit(Channel & ch, const ModeOp & op) const;
	bool	applyModeOperator(Channel & ch, Database & db, const ModeOp & op) const;

	void	recordChange(const ModeOp & op, std::string & outMode, std::vector<std::string> & outParams, char & currentSign) const;

};

// ------------------------------------------------
// function
// ------------------------------------------------

#endif
