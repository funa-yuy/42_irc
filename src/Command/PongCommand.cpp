#include "PongCommand.hpp"

PongCommand::PongCommand() {}

PongCommand::~PongCommand() {}

Command *	PongCommand::createPongCommand()
{
	return (new PongCommand());
}

std::vector<t_response>	PongCommand::execute(const t_parsed & input, Database & db) const
{
	std::vector<t_response>	responses;
	t_response				res;

	res.is_success = false;
	res.should_send = false;
	res.should_disconnect = false;
	res.reply = "";

	Client *	sender_client = db.getClient(input.client_fd);
	if (!sender_client)
	{
		responses.push_back(res);
		return (responses);
	}

	if(!isValidCmd(input, res, *sender_client))
	{
		responses.push_back(res);
		return (responses);
	}

	res.is_success = true;
	res.should_send = true;
	std::string	token = input.args[0];
	res.reply = "PONG :" + token + "\r\n";
	res.target_fds.push_back(input.client_fd);

	responses.push_back(res);
	return (responses);
}

bool	PongCommand::isValidCmd(const t_parsed & input, t_response & res, Client & client) const
{
	if (input.args.empty())
	{
		res.should_send = true;
		res.reply = ":ft.irc 409 " + client.getNickname() + " :No origin specified\r\n";
		res.target_fds.resize(1);
		res.target_fds[0] = input.client_fd;
		return (false);
	}
	if (input.args.size() > 0 && input.args[0] != "ft.irc")
	{
		res.should_send = true;
		res.reply = ":ft.irc 402 " + client.getNickname() + " " + input.args[0] + " :No such server\r\n";
		res.target_fds.resize(1);
		res.target_fds[0] = input.client_fd;
		return (false);
	}

	return (true);
}
