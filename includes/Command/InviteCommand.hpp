#ifndef INVITECOMMAND_HPP
# define INVITECOMMAND_HPP

#include "Command.hpp"

class InviteCommand : public Command
{

public:

	InviteCommand();
	~InviteCommand();

	static Command*			createInviteCommand();
	std::vector<t_response>	execute(const t_parsed& input, Database& db) const;

private:

	bool		isValidCmd(const t_parsed & input, t_response & res, Database & db) const;
	t_response	makeRplInviting(Client & inviter, Client & invitee, const std::string chName) const;
	t_response	makeInviteLine(Client & inviter, Client & invitee, const std::string chName) const;

};

#endif
