#include "Command/CapCommand.hpp"

CapCommand::CapCommand() {}

CapCommand::~CapCommand() {}

Command *	CapCommand::createCapCommand() { return (new CapCommand); }

std::vector<t_response>	CapCommand::execute(const t_parsed & input, Database & db) const
{
	std::vector<t_response>	responses;
	t_response				res;

	res.is_success = false;
	res.should_send = false;
	res.should_disconnect = false;

	Client *	sender_client = db.getClient(input.client_fd);
	if (!sender_client)
	{
		responses.push_back(res);
		return (responses);
	}

	if (!isValidCmd(input, res))
	{
		responses.push_back(res);
		return (responses);
	}

	responses.push_back(res);
	return (responses);
}

bool	CapCommand::isValidCmd(const t_parsed & input, t_response & res) const
{
	res.target_fds.push_back(input.client_fd);

	if (input.args.size() > 0 && input.args[0] == "LS")
	{
		res.should_send = true;
		res.reply = "CAP * LS :\r\n";
		return (true);
	}
	
	res.reply = "";
	return (false);
}
