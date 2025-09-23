#include "Command/ModeCommand.hpp"

static bool			isDigits(const std::string & s);
static int			findFdByNickInChannel(Database & db, Channel & ch, const std::string & nick);
static std::string	uIntToString(unsigned int num);

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

	// 変更の権限チェック
	if (!checkModifyPermissions(*ch, client, input.client_fd, res, chName))
		return (false);

	// 構文解析と引数の数チェック
	const std::string &	modeStr = input.args[1];
	if (modeStr.empty())
	{
		res.reply = ":ft.irc 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
		res.target_fds.push_back(input.client_fd);
		return (false);
	}
	std::vector<std::string> params;
	if (input.args.size() > 2)
		params.assign(input.args.begin() + 2, input.args.end());
	std::vector<ModeOp> ops;
	if (!parseModesAndParams(modeStr, params, ops, res, client))
		return (false);

	// 意味検証
	if (!validateSemantic(ops, *ch, db, client, chName, res))
		return (false);

	return (true);
}

bool	ModeCommand::checkModifyPermission(Channel & ch, const Client & client, int fd, t_response & res, const std::string & chName) const
{
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
	return (true);
}

bool	ModeCommand::parseModesAndParams(const std::string & modeStr, const std::vecotr<std::string> & params, std::vector<ModeOp> & ops, t_response & res, const Client & client)
{
	char	sign = 0;
	size_t	paramIndex = 0;

	for (size_t i = 0; i < modeStr.size(); ++i)
	{
		char c = modeStr[i];
		if (c == '+' || c == '-')
		{
			sign = c;
			continue ;
		}

		if (sign == 0 || !isKnownMode(c))
		{
			res.reply = ":ft.irc 472 " + client.getNickname() + " " + std::string(1, c) + " :is unknown mode channel to me\r\n";
			return (false);
		}
		if (needsParameter(c, sign))
		{
			if (paramIndex >= params.size())
			{
				res.reply = "ft.irc 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
				return (false);
			}
			ops.push_back(ModeOp(sign, c, params[paramIndex++]));
		}
		else
		{
			ops.push_back(ModeOp(sign, c, ""));
		}
	}
	return (true);
}

bool	ModeCommand::validateSemantic(const std::vector<ModeOp> & op, Channel & ch, Database & db, const Client & client, const std::string & chName, t_response & res) const
{
	for (size_t i = 0; i < ops.size(); ++i)
	{
		const ModeOp & op = ops[i];
		switch (op.mode)
		{
		case 'l':
			if (op.sign == '+')
			{
				if (!isDigits(op.param))
				{
					res.reply = ":ft.irc 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
					return (false);
				}
			}
			break ;
		case 'k':
			if (op.param.empty())
			{
				res.reply = ":ft.irc 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
				return (false);
			}
			break ;
		case 'o':
		{
			int fd = findFdByNickInChannel(db, ch, op.param);
			if (fd < 0)
			{
				res.reply = ":ft.irc 441 " + client.getNickname() + " " + op.param + " " + chName + " :Not on channel\r\n";
				return (false);
			}
			break ;
		}
		
		default:
			break ;
		}
	}

	return (true);
}

void	ModeCommand::buildChannelModeReply(const Channel & ch, std::string & modes, std::vector<std::string> & params) const
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
		params.push_back(uIntToString(ch.getLimit()));
	}
}

bool	ModeCommand::isKnownMode(char c) const
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

bool	ModeCommand::needsParameter(char c, char sign) const
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

static std::string	uIntToString(unsigned int num)
{
	std::ostringstream oss;
	oss << num;
	return (oss.str());
}
