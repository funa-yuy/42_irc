#include "Command/UserCommand.hpp"

UserCommand::UserCommand() {}

UserCommand::~UserCommand() {}

const t_response	UserCommand::execute(const t_parsed& input, Database& db) const {
	(void)input;
	(void)db;
	t_response	res;

	res.reply = ":servername Hi! USER!!!\r\n";
	return (res);
}
