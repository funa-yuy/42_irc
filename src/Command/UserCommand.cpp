#include "irc.hpp"
#include "Command/UserCommand.hpp"

UserCommand::UserCommand() {}

UserCommand::~UserCommand() {}

Command *	UserCommand::createUserCommand(void)
{
	return (new UserCommand());
}

const t_response	UserCommand::execute(const t_parsed& input, Database& db) const
{
	t_response	res;
	Client *	sender_client = db.getClient(input.client_fd);
	if (!sender_client)
	{
		res.is_success = false;
		res.should_send = false;
		return (res);
	}

	if (!isValidCmd(input, &res, sender_client))
		return (res);

	std::string username = input.args[0];
	if (username.length() > USERLEN)
		username = username.substr(0, USERLEN);
	
	sender_client->setUsername(username);

	std::string realname = input.args[3];
	if (!realname.empty() && realname[0] == ':')
		realname = realname.substr(1);
	if (realname.empty())
		realname = DEFAULT_REALNAME;
		
	sender_client->setRealname(realname);
	sender_client->setUserReceived(true);

	res.is_success = true;
	res.should_send = false;

	return (res);
}

bool	UserCommand::isValidCmd(const t_parsed & input, t_response * res, Client * client) const
{
	if (input.args.size() < 4)
	{
		res->is_success = false;
		res->should_send = true;
		res->reply = ":ft.irc 461 " + client->getNickname() + " USER :Not enough parameters\r\n";
		res->target_fds.resize(1);
		res->target_fds[0] = input.client_fd;
		return (false);
	}
	else if (client->getUserReceived())
	{
		res->is_success = false;
		res->should_send = true;
		res->reply = ":ft.irc 462 " + client->getNickname() + " :You may not reregister\r\n";
		res->target_fds.resize(1);
		res->target_fds[0] = input.client_fd;
		return (false);
	}

	return (true);
}
