#include "PassCommand.hpp"

PassCommand::PassCommand() {}

PassCommand::~PassCommand() {}

const t_response	PassCommand::execute(const t_parsed& input) const {
	(void)input;
	t_response	res;

	res.reply = ":servername Hi! PASS!!!\r\n";
	return (res);
}
