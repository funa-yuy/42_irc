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
	if (!input.args[1].empty())
		msg = input.args[1];
	else
		msg = "Client Quit";
	res.reply = ":" + sender->getNickname() + "@" + sender->getUsername() + "ft.irc QUIT :" + msg + "\r\n";
	res.target_fds.push_back(input.client_fd);
	responses.push_back(res);

	return (responses);
}

// bool	QuitCommand::isValidCmd(const t_parsed & input, t_response & res, Client & client) const
// {
// 	return (true);
// }
