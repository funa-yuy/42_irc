#ifndef KICKCOMMAND_HPP
# define KICKCOMMAND_HPP

#include "Command.hpp"

struct s_kick_item {
	std::string	channel;
	std::string	target;
	std::string	comment;
};

class KickCommand : public Command
{

public:

	KickCommand();
	~KickCommand();

	static Command*			createKickCommand();
	std::vector<t_response>	execute(const t_parsed& input, Database& db) const;

private:
	std::vector<s_kick_item>	parse_kick_args(const t_parsed& input) const;
	bool		isValidParamsSize(const t_parsed& input, t_response& res, Database& db) const;
	bool		isValidCmd(const t_parsed & input, t_response & res, Database & db, const std::string& chName, const std::string& targetNick) const;
	t_response	executeKick(const t_parsed& input, Database& db, const s_kick_item& item) const;
	void		updateDatabase(Database& db, const s_kick_item& item) const;
	t_response	makeKickBroadcast(const t_parsed& input, Database& db, const s_kick_item& item) const;
};

#endif
