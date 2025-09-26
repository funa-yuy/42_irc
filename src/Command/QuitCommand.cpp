#include "Command/QuitCommand.hpp"

QuitCommand::QuitCommand() {}

QuitCommand::~QuitCommand() {}

QuitCommand::createQuitCommand() { return ( new QuitCommand() ); }

std::vector<t_response>	QuitCommand::execute(const t_parsed & input, Database & db) const
{
	// std::vector<t_response>	responses;
	// t_response				res;

	// res.should_send = true;
	// res.should_disconnect = true;
	
	// Client * sender = db.getClient(input.client_fd);
	// if (!sender)
	// {
	// 	res.should_send = false;
	// 	res.should_disconnect = false;
	// 	return (responses);
	// }

	// res.is_success = true;
}

bool	QuitCommand::isValidCmd(const t_parsed & input, t_response & res, Client & client) const
{

}
