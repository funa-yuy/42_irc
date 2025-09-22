#include "Command/TopicCommand.hpp"

TopicCommand::TopicCommand() {}

TopicCommand::~TopicCommand() {}

Command*	TopicCommand::createTopicCommand() {
	return (new TopicCommand());
}


t_response	TopicCommand::makeRplBroadcast(Client& client, Channel& ch) const
{
	t_response	res;
	std::string nick = client.getNickname();
	std::string user = client.getUsername();
	std::string source = ":" + nick + "!" + user + "@ft.irc";

	res.is_success = true;
	res.should_send = true;
	res.should_disconnect = false;
	res.reply = source + " TOPIC " + ch.getName() + " :" + ch.getTopic() + "\r\n";
	res.target_fds.assign(ch.getClientFds().begin(), ch.getClientFds().end());
	return (res);
}

t_response	TopicCommand::makeRplNotopic(Client& client, const std::string& chName) const
{
	t_response	res;

	res.is_success = true;
	res.should_send = true;
	res.should_disconnect = false;
	res.reply = "ft.irc 331 " + client.getNickname() + " " + chName + " :No topic is set\r\n";
	res.target_fds.push_back(client.getFd());

	return (res);
}

t_response	TopicCommand::makeRplTopic(Client& client, Channel& ch) const
{
	t_response	res;

	res.is_success = true;
	res.should_send = true;
	res.should_disconnect = false;
	res.reply = "ft.irc 332 " + client.getNickname() + " " + ch.getName() + " :" + ch.getTopic() + "\r\n";
	res.target_fds.push_back(client.getFd());

	return (res);
}

t_response	TopicCommand::makeRplTopicWhoTime(Client& client, Channel& ch) const
{
	t_response	res;

	res.is_success = true;
	res.should_send = true;
	res.should_disconnect = false;
	// res.reply = "ft.irc 333 " + client.getNickname() + " " + ch.getName() + " " + ch.getTopicWho() + " " + ch.getTopicTime() + "\r\n";
	res.reply = "ft.irc 333 " + client.getNickname() + " " + ch.getName() + "\r\n";
	res.target_fds.push_back(client.getFd());

	return (res);
}

std::vector<t_response>	TopicCommand::execute(const t_parsed & input, Database & db) const
{
	std::vector<t_response>	responses;
	t_response				res;

	if (!isValidCmd(input, &res, db))
	{
		responses.push_back(res);
		return (responses);
	}

	Channel*	ch = db.getChannel(input.args[0]);
	Client* client = db.getClient(input.client_fd);
	if (input.args.size() == 1) {
		if (ch->getTopic().empty()) {
			responses.push_back(makeRplNotopic(*client, ch->getName()));
		} else {
			responses.push_back(makeRplTopic(*client, *ch));
			responses.push_back(makeRplTopicWhoTime(*client, *ch));
		}

	}
	else {
		ch->setTopic(input.args[1]);
		//whoのset
		//timeのsetat
		responses.push_back(makeRplBroadcast(*client, *ch));
	}

	return (responses);
}

static void	set_err_res(t_response *res, const t_parsed& input, std::string errmsg) {
	res->is_success = false;
	res->should_send = true;
	res->should_disconnect = false;
	res->reply = ":ft.irc " + errmsg + "\r\n";
	res->target_fds.resize(1);
	res->target_fds[0] = input.client_fd;
}

bool	TopicCommand::isValidCmd(const t_parsed & input, t_response* res, Database & db) const
{
	Client*	client = db.getClient(input.client_fd);
	if (input.args.size() < 1)
	{
		set_err_res(res, input, ":ft.irc 461 " + client->getNickname() + " TOPIC :Not enough parameters\r\n");
		return (false);
	}

	Channel*	ch = db.getChannel(input.args[0]);
	if (ch == NULL)
	{
		set_err_res(res, input, ":ft.irc 403 " + client->getNickname() + " " + input.args[0] + " :No such channel\r\n");
		return (false);
	}
	if (!ch->isMember(input.client_fd))
	{
		set_err_res(res, input, ":ft.irc 442 " + client->getNickname() + " " + input.args[0] + " :You're not on that channel\r\n");
		return (false);
	}
	if (ch->getTopicRestricted() && !ch->isOperator(input.client_fd))
	{
		set_err_res(res, input, ":ft.irc 482 " + client->getNickname() + " " + input.args[0] + " :You're not channel operator\r\n");
		return (false);
	}
	return (true);
}
