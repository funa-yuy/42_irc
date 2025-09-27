#ifndef KICKCOMMAND_HPP
# define KICKCOMMAND_HPP

#include "Command.hpp"

class KickCommand : public Command
{

public:

	KickCommand();
	~KickCommand();

	static Command*			createKickCommand();
	std::vector<t_response>	execute(const t_parsed& input, Database& db) const;

private:

	bool		isValidCmd(const t_parsed & input, t_response & res, Database & db) const;
	t_response	makeKickBroadcast(const Client& kicker, const std::string& chName, const Client& target, const std::string& comment, const std::set<int>& clientFds) const;
};

#endif

