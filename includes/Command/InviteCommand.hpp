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

};

#endif
