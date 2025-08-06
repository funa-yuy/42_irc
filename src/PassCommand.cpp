#include "PassCommand.hpp"

PassCommand::PassCommand() {}

PassCommand::~PassCommand() {}

const t_response	PassCommand::execute(const t_parserd& input) const {
	(void)input;
	t_response	res;

	res.reply = ":servername Hi! PASS!!!NG!\r\n";
	return (res);
}
