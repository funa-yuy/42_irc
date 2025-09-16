#include "Command/InviteCommand.hpp"

InviteCommand::InviteCommand() {}

InviteCommand::~InviteCommand() {}

Command *	InviteCommand::createInviteCommand() { return (new InviteCommand()); }

std::vector<t_response>	InviteCommand::execute(const t_parsed & input, Database & db) const
{
	std::vector<t_response>	responses;
	t_response				res;

	responses.push_back(res);
	return (responses);
}
