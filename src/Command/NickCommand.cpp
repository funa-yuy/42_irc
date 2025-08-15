#include "NickCommand.hpp"

NickCommand::NickCommand() {}

NickCommand::~NickCommand() {}

const t_response	NickCommand::execute(const t_parsed& input) const {
	(void)input;
	t_response	res;

	res.reply = ":servername Hi! NICK!!!\r\n";
	return (res);
}
