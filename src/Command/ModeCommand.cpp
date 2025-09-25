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
		res.should_send = true;
		res.target_fds.push_back(input.client_fd);
		responses.push_back(res);
		return (responses);
	}

	const std::string & chName = input.args[0];
	Channel * ch = db.getChannel(chName);
	if (!ch)
	{
		responses.push_back(res);
		return (responses);
	}

	res.is_success = true;

	if (input.args.size() == 1)
		return (handleModeView(*sender_client, *ch));
	else
		return (handleModeChange(input, db, *sender_client, *ch));
}

bool	ModeCommand::isValidCmd(const t_parsed & input, t_response & res, Client & client, Database & db) const
{
	res.should_send = true;

	if (input.args.empty())
	{
		res.reply = ":ft.irc 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
		return (false);
	}

	const std::string & chName = input.args[0];
	Channel * ch = db.getChannel(chName);
	if (!ch)
	{
		res.reply = ":ft.irc 403 " + client.getNickname() + " " + chName + " :No such channel\r\n";
		return (false);
	}

	if (input.args.size() == 1)
		return (true);

	const std::string &	modeStr = input.args[1];
	if (modeStr.empty())
	{
		res.reply = ":ft.irc 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
		return (false);
	}

	return (true);
}

std::vector<t_response>	ModeCommand::handleModeView(Client & sender, Channel & ch) const
{
    std::vector<t_response> responses;
    t_response res;

    res.is_success = true;
    res.should_send = false;
    res.should_disconnect = false;

    const std::string& chName = ch.getName();

	if (!ch.isMember(sender.getFd()))
	{
		res.should_send = true;
		res.reply = ":ft.irc 442 " + sender.getNickname() + " " + chName + " :You're not on that channel\r\n";
		res.target_fds.push_back(sender.getFd());
		responses.push_back(res);
		return (responses);
	}

	std::string	modes;
	std::vector<std::string> params;
	buildChannelModeReply(ch, modes, params);

	res.should_send = true;
	res.reply = ":ft.irc 324 " + sender.getNickname() + " " + chName + " " + modes;
	for (size_t i = 0; i < params.size(); ++i)
		res.reply += " " + params[i];
	res.reply += "\r\n";
	res.target_fds.push_back(sender.getFd());
	responses.push_back(res);
	return (responses);
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

std::vector<t_response>	ModeCommand::handleModeChange(const t_parsed & input, Database & db, Client & sender, Channel & ch) const
{
    std::vector<t_response> responses;
    t_response res;
    res.is_success = true;
    res.should_send = false;
    res.should_disconnect = false;

    const std::string& chName = ch.getName();

	std::vector<std::string> params;
	if (input.args.size() > 2)
		params.assign(input.args.begin() + 2, input.args.end());

	std::vector<ModeOp> ops;
	if (!parseModesAndParams(input.args[1], params, ops, res, sender))
	{
		res.should_send = true;
		res.target_fds.push_back(sender.getFd());
		responses.push_back(res);
		return (responses);
	}
	if (ops.empty())
	{
		res.should_send = true;
		res.reply = ":ft.irc 461 " + sender.getNickname() + " MODE :Not enough parameters\r\n";
		res.target_fds.push_back(sender.getFd());
		responses.push_back(res);
		return (responses);
	}
	if (!validateSemantic(ops, ch, db, sender, chName, res))
	{
		res.should_send = true;
		res.target_fds.push_back(sender.getFd());
		responses.push_back(res);
		return (responses);
	}

	if (!checkPermissions(ch, sender, input.client_fd, res, chName))
	{
		res.should_send = true;
		res.target_fds.push_back(sender.getFd());
		responses.push_back(res);
		return (responses);
	}

	std::string appliedModes;
	std::vector<std::string> appliedParams;
	if (!applyModes(ch, db, ops, appliedModes, appliedParams))
		return (responses);

	res.should_send = true;
	res.reply = ":" + sender.getNickname() + "!" + sender.getUsername() + "@ft.irc" + " MODE " + chName + " " + appliedModes;
	for (size_t i = 0; i < appliedParams.size(); ++i)
		res.reply += " " + appliedParams[i];
	res.reply += "\r\n";
	const std::set<int> & fds = ch.getClientFds();
	for (std::set<int>::const_iterator it = fds.begin(); it != fds.end(); ++it)
		res.target_fds.push_back(*it);

	responses.push_back(res);

	return (responses);
}

bool	ModeCommand::parseModesAndParams(const std::string & modeStr, const std::vector<std::string> & params, std::vector<ModeOp> & ops, t_response & res, const Client & client) const
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
			res.reply = ":ft.irc 472 " + client.getNickname() + " " + std::string(1, c) + " :is unknown mode char to me\r\n";
			return (false);
		}
		if (needsParameter(c, sign))
		{
			if (paramIndex >= params.size())
			{
				res.reply = ":ft.irc 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
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
		return (sign == '+');
	else if (c == 'l')
		return (sign == '+');
	return (false);
}

bool	ModeCommand::validateSemantic(const std::vector<ModeOp> & ops, Channel & ch, Database & db, const Client & client, const std::string & chName, t_response & res) const
{
	for (size_t i = 0; i < ops.size(); ++i)
	{
		const ModeOp & op = ops[i];
		switch (op.mode)
		{
			case 'l':
				if (op.sign == '+')
				{
					if (!isDigits(op.param) || op.param == "0")
					{
						res.reply = ":ft.irc 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
						return (false);
					}
				}
				break ;
			case 'k':
				if (op.sign == '+')
				{
					if (op.param.empty())
					{
						res.reply = ":ft.irc 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
						return (false);
					}
					if (ch.getHasKey())
					{
						res.reply = ":ft.irc 467 " + client.getNickname() + " " + chName + " :Channel key already set\r\n";
						return (false);
					}
				}
				break ;
			case 'o':
			{
				int fd = findFdByNickInChannel(db, ch, op.param);
				if (fd < 0)
				{
					res.reply = ":ft.irc 441 " + client.getNickname() + " " + op.param + " " + chName + " :They aren't on that channel\r\n";
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

bool	ModeCommand::checkPermissions(Channel & ch, const Client & client, int fd, t_response & res, const std::string & chName) const
{
	if (!ch.isMember(fd))
	{
		res.reply = ":ft.irc 442 " + client.getNickname() + " " + chName + " :You're not on that channel\r\n";
		return (false);
	}
	if (!ch.isOperator(fd))
	{
		res.reply = ":ft.irc 482 " + client.getNickname() + " " + chName + " :You're not channel operator\r\n";
		return (false);
	}
	return (true);
}

bool	ModeCommand::applyModes(Channel & ch, Database & db, const std::vector<ModeOp> & ops, std::string & outModes, std::vector<std::string> & outParams) const
{
	outModes.clear();
	outParams.clear();

	bool	anyChanged = false;
	char	currentSign = 0;

	for (size_t i = 0; i < ops.size(); ++i)
	{
		const ModeOp &	op = ops[i];
		bool			changed = false;

		switch (op.mode)
		{
			case 'i':
				changed = applyModeInviteOnly(ch, op);
				break ;
			case 't':
				changed = applyModeTopicRestricted(ch, op);
				break ;
			case 'k':
				changed = applyModeKey(ch, op);
				break ;
			case 'l':
				changed = applyModeLimit(ch, op);
				break ;
			case 'o':
				changed = applyModeOperator(ch, db, op);
				break ;
			default:
				break ;
		}

		if (!changed)
			continue ;

		anyChanged = true;
		recordChange(op, outModes, outParams, currentSign);
	}

	return (anyChanged);
}

bool	ModeCommand::applyModeInviteOnly(Channel & ch, const ModeOp & op) const
{
	if (op.sign == '+')
	{
		if (!ch.getInviteOnly())
		{
			ch.setInviteOnly(true);
			return (true);
		}
	}
	else
	{
		if (ch.getInviteOnly())
		{
			ch.setInviteOnly(false);
			return (true);
		}
	}
	return (false);
}

bool	ModeCommand::applyModeTopicRestricted(Channel & ch, const ModeOp & op) const
{
	if (op.sign == '+')
	{
		if (!ch.getTopicRestricted())
		{
			ch.setTopicRestricted(true);
			return (true);
		}
	}
	else
	{
		if (ch.getTopicRestricted())
		{
			ch.setTopicRestricted(false);
			return (true);
		}
	}
	return (false);
}

bool	ModeCommand::applyModeKey(Channel & ch, const ModeOp & op) const
{
	if (op.sign == '+')
	{
		if (!ch.getHasKey() || ch.getKey() != op.param)
		{
			ch.setKey(op.param);
			return (true);
		}
	}
	else
	{
		if(ch.getHasKey())
		{
			ch.clearKey();
			return (true);
		}
	}
	return (false);
}

bool	ModeCommand::applyModeLimit(Channel & ch, const ModeOp & op) const
{
	if (op.sign == '+')
	{
		unsigned int	lim = static_cast<unsigned int>(std::atoi(op.param.c_str()));
		if (!ch.getHasLimit() || ch.getLimit() != lim)
		{
			ch.setLimit(lim);
			return (true);
		}
	}
	else
	{
		if (ch.getHasLimit())
		{
			ch.clearLimit();
			return (true);
		}
	}
	return (false);
}

bool	ModeCommand::applyModeOperator(Channel & ch, Database & db, const ModeOp & op) const
{
	int	targetFd = findFdByNickInChannel(db, ch, op.param);
	if (targetFd >= 0)
	{
		if (op.sign == '+')
		{
			if (!ch.isOperator(targetFd))
			{
				ch.setChannelOperatorFds(targetFd);
				return (true);
			}
		}
		else
		{
			if (ch.isOperator(targetFd))
			{
				ch.removeChannelOperatorFd(targetFd);
				return (true);
			}
		}
	}
	return (false);
}

void	ModeCommand::recordChange(const ModeOp & op, std::string & outModes, std::vector<std::string> & outParams, char & currentSign) const
{
	if (currentSign != op.sign)
	{
		currentSign = op.sign;
		outModes.push_back(currentSign);
	}
	outModes.push_back(op.mode);

	if ((op.mode == 'k' && op.sign == '+') || op.mode == 'o' || (op.mode == 'l' && op.sign == '+'))
		outParams.push_back(op.param);

	return ;
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
