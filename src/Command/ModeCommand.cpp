#include "ModeCommand.hpp"

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
		// チャンネル参加 + オペレータ権限チェック
		std::string	modes = "+"; // todo: ch->getCurrentModes() を実装
		res.reply = ":ft.irc 324 " + sender_client->getNickname() + " " + chName + " " + modes + "\r\n";
		res.target_fds.push_back(input.client_fd);
		responses.push_back(res);
		return (responses);
	}

	return (responses);
}

bool	ModeCommand::isValidCmd(const t_parsed & input, t_response & res, Client & client, Database & db) const
{
	res.should_send = true;

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

	std::string &	modeStr = input.args[1];
	if (modeStr.empty())
	{
		res.reply = ":ft.irc 461 " + client.getNickname() + " MODE :Not enough parameters\r\n";
		res.target_fds.push_back(input.client_fd);
		return (false);
	}

	// エラー442と482チェック（チャンネル参加必須 + オペレータ必須）

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
		res.reply = ":ft.irc 461 " + client.getNickname() + "MODE :Not enough parameters\r\n";
		res.target_fds.push_back(input.client_fd);
		return (false);
	}

	return (true);
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
		return (sign == '+');
	else if (c == 'l')
		return (sign == '+');
	return (false);
}
