#include "NickCommand.hpp"

NickCommand::NickCommand() {}

NickCommand::~NickCommand() {}

const t_response	NickCommand::execute(const t_parsed& input, Database& db) const {
	t_response	res;
	bool		is_registed = false;

	if (input.args.size() != 1)
	{
		set_err_res(&res, input, "");
		return (res);
	}

	// 文字数のチェック
	if (1 < input.args[0].size() && input.args[0].size() < 10)
	{
		set_err_res(&res, input, "");
		return (res);
	}
	// 文字種類のチェック
	for (int i = 0;input.args[0][i] != NULL;i++)
	{
		input.args[0][i].isdigit();
	}

	if (db.getClient(input.client_fd))
		is_registed = true;

	std::string registerd_nick;
	for (int i = 0; db.getClient(i) != NULL; i++)
	{
		if (db.getClient(i)->getNickname() == input.args[0])
		{
			set_err_res(&res, input, "");
			return (res);
		}
	}
	res.is_success = true;
	res.should_send = false;
	return (res);
}

void	set_err_res(t_response *res, 
						const t_parsed& input, 
						std::string errmsg)
{
	res->is_success = false;
	res->should_send = true;
	res->reply = ":ft.irc " + errmsg + "\r\n";
	res->target_fds.resize(1);
	res->target_fds[0] = input.client_fd;
}
