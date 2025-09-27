#include "Command/KickCommand.hpp"

KickCommand::KickCommand() {}

KickCommand::~KickCommand() {}

Command *	KickCommand::createKickCommand() { return (new KickCommand()); }

static void	set_err_res(t_response &res, const t_parsed& input, const std::string& msg)
{
	res.is_success = false;
	res.should_send = true;
	res.should_disconnect = false;
	res.reply = ":ft.irc " + msg + "\r\n";
	res.target_fds.clear();
	res.target_fds.push_back(input.client_fd);
}

bool	KickCommand::isValidCmd(const t_parsed & input, t_response & res, Database & db) const
{
	Client *kicker = db.getClient(input.client_fd);
	if (!kicker)
	{
		res.should_send = false;
		return (false);
	}
	if (input.args.size() < 2)
	{
		set_err_res(res, input, "461 " + kicker->getNickname() + " KICK :Not enough parameters");
		return (false);
	}

	const std::string chName = input.args[0];
	Channel *ch = db.getChannel(chName);
	if (!ch)
	{
		set_err_res(res, input, "403 " + kicker->getNickname() + " " + chName + " :No such channel");
		return (false);
	}
	if (!ch->isOperator(kicker->getFd()))
	{
		set_err_res(res, input, "482 " + kicker->getNickname() + " " + chName + " :You're not channel operator");
		return (false);
	}
	if (!ch->isMember(kicker->getFd()))
	{
		set_err_res(res, input, "442 " + kicker->getNickname() + " " + chName + " :You're not on that channel");
		return (false);
	}

	const std::string targetNick = input.args[1];
	Client *target = db.getClient(const_cast<std::string&>(targetNick));
	if (!target || !ch->isMember(target->getFd()))
	{
		set_err_res(res, input, "441 " + kicker->getNickname() + " " + targetNick + " " + chName + " :They aren't on that channel");
		return (false);
	}

	res.is_success = true;
	return (true);
}

t_response	KickCommand::makeKickBroadcast(const Client& kicker, const std::string& chName, const Client& target, const std::string& comment, const std::set<int>& clientFds) const
{
	t_response res;
	res.is_success = true;
	res.should_send = true;
	res.should_disconnect = false;
	std::string source = ":" + kicker.getNickname() + "!" + kicker.getUsername() + "@ft.irc";
	std::string tail = comment.empty() ? "" : (" :" + comment);
	res.reply = source + " KICK " + chName + " " + target.getNickname() + tail + "\r\n";
	res.target_fds.assign(clientFds.begin(), clientFds.end());
	return (res);
}

std::vector<t_response>	KickCommand::execute(const t_parsed& input, Database& db) const
{
	std::vector<t_response> responses;
	t_response res;
	res.is_success = false;
	res.should_disconnect = false;
	if (!isValidCmd(input, res, db))
	{
		responses.push_back(res);
		return (responses);
	}

	Client *kicker = db.getClient(input.client_fd);
	const std::string chName = input.args[0];
	const std::string targetNick = input.args[1];
	std::string comment;
	if (input.args.size() >= 3) comment = input.args[2];

	Channel *ch = db.getChannel(chName);
	Client *target = db.getClient(const_cast<std::string&>(targetNick));
	if (!kicker || !ch || !target)
		return (responses);

	// remove and broadcast
	const std::set<int>& fds = ch->getClientFds();
	responses.push_back(makeKickBroadcast(*kicker, chName, *target, comment, fds));
	ch->removeClientFd(target->getFd());

	return (responses);
}


