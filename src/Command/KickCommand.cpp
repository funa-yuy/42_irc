#include "Command/KickCommand.hpp"
#include <sstream>

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

std::vector<s_kick_item>	KickCommand::parse_kick_args(const t_parsed& input) const
{
	std::vector<s_kick_item> items;
	std::string chName = input.args[0];
	std::string targetsCsv = input.args[1];
	std::string comment;
	if (input.args.size() >= 3)
		comment = input.args[2];

	std::vector<std::string> targets = split(targetsCsv, ',');
	for (size_t i = 0; i < targets.size(); ++i)
	{
		s_kick_item it;
		it.channel = chName;
		it.target = targets[i];
		it.comment = comment;
		items.push_back(it);
	}
	return (items);
}

bool	KickCommand::isValidParamsSize(const t_parsed& input, t_response& res, Database& db) const
{
	Client *kicker = db.getClient(input.client_fd);
	if (!kicker)
	{
		res.should_send = false;
		res.should_disconnect = false;
		return (false);
	}
	if (input.args.size() < 2)
	{
		set_err_res(res, input, "461 " + kicker->getNickname() + " KICK :Not enough parameters");
		return (false);
	}
	return (true);
}

bool	KickCommand::isValidCmd(const t_parsed & input, t_response & res, Database & db, const std::string& chName, const std::string& targetNick) const
{
	Client *kicker = db.getClient(input.client_fd);
	Channel *ch = db.getChannel(chName);
	if (!ch)
	{
		set_err_res(res, input, "403 " + kicker->getNickname() + " " + chName + " :No such channel");
		return (false);
	}
	if (!ch->isMember(kicker->getFd()))
	{
		set_err_res(res, input, "442 " + kicker->getNickname() + " " + chName + " :You're not on that channel");
		return (false);
	}
	if (!ch->isOperator(kicker->getFd()))
	{
		set_err_res(res, input, "482 " + kicker->getNickname() + " " + chName + " :You're not channel operator");
		return (false);
	}

	Client *target = db.getClient(const_cast<std::string&>(targetNick));
	if (!target || !ch->isMember(target->getFd()))
	{
		set_err_res(res, input, "441 " + kicker->getNickname() + " " + targetNick + " " + chName + " :They aren't on that channel");
		return (false);
	}
	res.is_success = true;
	return (true);
}

t_response	KickCommand::makeKickBroadcast(const t_parsed& input, Database& db, const s_kick_item& item) const
{
	t_response res;
	Client *kicker = db.getClient(input.client_fd);
	Channel *ch = db.getChannel(item.channel);
	Client *target = db.getClient(const_cast<std::string&>(item.target));
	const std::set<int>& fds = ch->getClientFds();

	res.is_success = true;
	res.should_send = true;
	res.should_disconnect = false;
	std::string source = ":" + kicker->getNickname() + "!" + kicker->getUsername() + "@ft.irc";
	std::string comment;
	if (!item.comment.empty()) {
		comment += " :";
		comment += item.comment;
	}
	res.reply = source + " KICK " + item.channel + " " + target->getNickname() + comment + "\r\n";
	res.target_fds.assign(fds.begin(), fds.end());
	return (res);
}

void	KickCommand::updateDatabase(Database& db, const s_kick_item& item) const
{
	Channel *ch = db.getChannel(item.channel);
	if (!ch)
		return ;
	Client *target = db.getClient(const_cast<std::string&>(item.target));
	if (!target)
		return ;
	ch->removeClientFd(target->getFd());
}

t_response	KickCommand::executeKick(const t_parsed& input, Database& db, const s_kick_item& item) const
{
	t_response res;
	if (!isValidCmd(input, res, db, item.channel, item.target))
		return (res);
	res = makeKickBroadcast(input, db, item);
	updateDatabase(db, item);
	return (res);
}

std::vector<t_response>	KickCommand::execute(const t_parsed& input, Database& db) const
{
	std::vector<t_response> response_list;
	t_response res;
	if (!isValidParamsSize(input, res, db))
	{
		response_list.push_back(res);
		return (response_list);
	}

	std::vector<s_kick_item> items = parse_kick_args(input);

	for (size_t i = 0; i < items.size(); ++i)
	{
		response_list.push_back(executeKick(input, db, items[i]));
	}

	return (response_list);
}


