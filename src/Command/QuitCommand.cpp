#include "Command/QuitCommand.hpp"

QuitCommand::QuitCommand() {}

QuitCommand::~QuitCommand() {}

Command *	QuitCommand::createQuitCommand() { return ( new QuitCommand() ); }

std::vector<t_response>	QuitCommand::execute(const t_parsed & input, Database & db) const
{
	std::vector<t_response>	responses;
	t_response				res;

	res.is_success = false;
	res.should_send = true;
	res.should_disconnect = true;
	res.target_fds.clear();
	res.reply.clear();
	
	Client * sender = db.getClient(input.client_fd);
	if (!sender)
	{
		res.should_send = false;
		res.should_disconnect = false;
		return (responses);
	}

	res.is_success = true;
	std::string	msg;
	if (input.args.size() > 0 && !input.args[0].empty())
		msg = input.args[0];
	else
		msg = "Client quit";
	
	
	responses.push_back(makeBroadcastQuit(msg, *sender, db));
	responses.push_back(makeRplError(msg, *sender));

	return (responses);
}

t_response	QuitCommand::makeBroadcastQuit(std::string & msg, Client & sender, Database & db) const
{
	t_response	broadcast;

	broadcast.should_send = true;
	broadcast.should_disconnect = false;

	std::string	prefix = ":" + sender.getNickname() + "!" + sender.getUsername() + "@ft.irc";
	broadcast.reply = prefix + " QUIT :" + msg + "\r\n";

	std::set<int>				targets;
	std::vector<std::string>	names = db.getAllChannelNames();

	for (size_t i = 0; i < names.size(); ++i)
	{
		Channel * ch = db.getChannel(names[i]);
		if (!ch)
			continue ;
		if (!ch->isMember(sender.getFd()))
			continue ;
		const std::set<int> & fds = ch->getClientFds();
		targets.insert(fds.begin(), fds.end());
	}
	targets.erase(sender.getFd());
	broadcast.target_fds.assign(targets.begin(), targets.end());

	broadcast.is_success = true;
	return (broadcast);
}

t_response	QuitCommand::makeRplError(const std::string & msg, Client & sender) const
{
	t_response	res;

	res.should_send = true;
	res.should_disconnect = true;

	res.reply = "ERROR :Closing Link: " + sender.getNickname() + " [Quit: " + msg + "]\r\n";
	res.target_fds.clear();
	res.target_fds.push_back(sender.getFd());

	res.is_success = true;
	return (res);
}
