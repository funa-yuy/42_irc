#include "ModeCommand.hpp"

static bool	isDigits(const std::string & s);
static int	findFdByNickInChannel(Database & db, Channel & ch, const std::string & nick);

ModeCommand::ModeCommand() {}

ModeCommand::~ModeCommand() {}

Command *	ModeCommand::createModeCommand() { return (new ModeCommand()); }

std::vector<t_response>	ModeCommand::execute(const t_parsed & input, Database & db) const
{
	std::vector<t_response>	responses;
	t_response				res;

	res.is_success = false;
	res.should_send = false;
	res.should_disconnect = false;
	res.reply.clear();
	res.target_fds.clear();

	Client *	sender_client = db.getClient(input.client_fd);
	if (!sender_client)
	{
		responses.push_back(res);
		return (responses);
	}

	if (!isValidCmd(input, res, *sender_client, db))
	{
		responses.push_back(res);
		return (responses);
	}

	std::string	chName = input.args[0];
	Channel *	ch = db.getChannel(chName);
	if (!ch)
	{
		responses.push_back(res);
		return (responses);
	}

	res.is_success = true;

	if (input.args.size() == 1)
	{
		if (!ch->isMember(input.client_fd))
		{
			res.should_send = true;
			res.reply = ":ft.irc 442 " + sender_client->getNickname() + " " + chName + " :You're not on that channel\r\n";
			res.target_fds.push_back(input.client_fd);
			responses.push_back(res);
			return (responses);
		}
		if (!ch->isOperator(input.client_fd))
		{
			res.should_send = true;
			res.reply = ":ft.irc 482 " + sender_client->getNickname() + " " + chName + " :You're not channel operator\r\n";
			res.target_fds.push_back(input.client_fd);
			responses.push_back(res);
			return (responses);
		}

		std::string	modes;
		std::vector<std::string> params;
		buildChannelModeReply(*ch, modes, params);

		res.should_send = true;
		res.reply = ":ft.irc 324 " + sender_client->getNickname() + " " + chName + " " + modes;
		for (size_t i = 0; i < params.size(); ++i)
			res.reply += " " + params[i];
		res.reply += "\r\n";
		res.target_fds.push_back(input.client_fd);
		responses.push_back(res);
		return (responses);
	}

	return (responses);
}

bool	ModeCommand::isValidCmd(const t_parsed & input, t_response & res, Client & client, Database & db) const
{
	res.should_send = true;

	if (input.args.empty())
	{
		res.reply = ":ft.irc 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
		res.target_fds.push_back(input.client_fd);
		return (false);
	}

	std::string	chName = input.args[0];
	Channel *	ch = db.getChannel(chName);
	if (!ch)
	{
		res.reply = ":ft.irc 403 " + client.getNickname() + " " + chName + " :No such channel\r\n";
		res.target_fds.push_back(input.client_fd);
		return (false);
	}

	if (input.args.size() == 1)
		return (true);

	if (!ch->isMember(input.client_fd))
	{
		res.reply = ":ft.irc 442 " + client.getNickname() + " " + chName + " :You're not on that channel\r\n";
		res.target_fds.push_back(input.client_fd);
		return (false);
	}
	if (!ch->isOperator(input.client_fd))
	{
		res.reply = ":ft.irc 482 " + client.getNickname() + " " + chName + " :You're not channel operator\r\n";
		res.target_fds.push_back(input.client_fd);
		return (false);
	}

	const std::string &	modeStr = input.args[1];
	if (modeStr.empty())
	{
		res.reply = ":ft.irc 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
		res.target_fds.push_back(input.client_fd);
		return (false);
	}

	char	sign = 0;
	size_t	needsParams = 0;

	for (size_t i = 0; i < modeStr.size(); ++i)
	{
		char	c = modeStr[i];
		if (c == '+' || c == '-')
		{
			sign = c;
			continue ; 
		}

		if (sign == 0 || !isKnownMode(c))
		{
			res.reply = ":ft.irc 472 " + client.getNickname() + " " + std::string(1, c) + " :is unknown mode char to me\r\n";
			res.target_fds.push_back(input.client_fd);
			return (false);
		}

		if (needsParameter(c, sign))
			++needsParams;
	}

	size_t	givenParams = 0;
	if (input.args.size() > 2)
		givenParams = input.args.size() - 2;

	if (givenParams < needsParams)
	{
		res.reply = ":ft.irc 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
		res.target_fds.push_back(input.client_fd);
		return (false);
	}

	sign = 0;
	size_t	paramIndex = 2;
	for (size_t i = 0; i < modeStr.size(); ++i)
	{
		char c = modeStr[i];
		if (c == '+' || c == '-')
		{
			sign = c;
			continue ;
		}

		if (c == 'l' && sign == '+')
		{
			std::string & limStr = input.args[paramIndex++];
			if (!isDigits(limStr))
			{
				res.reply = ":ft.irc 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
				res.target_fds.push_back(input.client_fd);
				return (false);
			}
		}
		else if (c == 'k')
		{
			std::string & key = input.args[paramIndex++];
			if (key.empty())
			{
				res.reply = ":ft.irc 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
				res.target_fds.push_back(input.client_fd);
				return (false);
			}
		}
		else if (c == 'o')
		{
			std::string & nick = input.args[paramIndex++];
			int fd = findFdByNickInChannel(db, *ch, nick);
			if (fd < 0)
			{
				res.reply = ":ft.irc 441 " + client.getNickname() + " " + nick + " " + chName + " :They aren't on that channel\r\n";
				res.target_fds.push_back(input.client_fd);
				return (false);
			}
		}
	}

	return (true);
}

void	ModeCommand::buildChannelModeReply(Channel & ch, std::string & modes, std::vector<std::string> & params)
{
	modes.clear();
	params.clear();
	modes.push_back('+');
	if (ch.getInviteOnly())
		modes.push_back('i');
	if (ch.getTopicRestricted())
		modes.push_back('t');
	if (ch.getHasKey())
	{
		modes.push_back('k');
		params.push_back(ch.getKey());
	}
	if (ch.getHasLimit())
	{
		modes.push_back('l');
		params.push_back(toString(ch.getLimit()));
	}
}

bool	ModeCommand::isKnownMode(char c)
{
	switch (c)
	{
		case 'i':
		case 't':
		case 'k':
		case 'o':
		case 'l':
			return (true);
		default:
			return (false);
	}
}

bool	ModeCommand::needsParameter(char c, char sign)
{
	if (c == 'o')
		return (true);
	else if (c == 'k')
		return (true);
	else if (c == 'l')
		return (sign == '+');
	return (false);
}

static bool	isDigits(const std::string & s)
{
	if (s.empty())
		return (false);
	for (size_t i = 0; i < s.size(); ++i)
	{
		if (!std::isdigit(static_cast<unsigned char>(s[i])))
			return (false);
	}
	return (true);
}

static int	findFdByNickInChannel(Database & db, Channel & ch, const std::string & nick)
{
	const std::set<int> & fds = ch.getClientFds();
	for (std::set<int>::const_iterator it = fds.begin(); it != fds.end(); ++it)
	{
		Client * c = db.getClient(*it);
		if (!c)
			continue ;
		if (c->getNickname() == nick)
			return (*it);
	}
	return (-1);
}
