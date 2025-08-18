#include "Command/NickCommand.hpp"
#include "irc.hpp"

NickCommand::NickCommand() {}

NickCommand::~NickCommand() {}

const t_response	NickCommand::execute(const t_parsed& input, Database& db) const {
	t_response	res;
	bool		is_registed = false;

	if (input.args.size() == 0 || input.args[0].size() < 1)
	{
		set_err_res(&res, input, "431 :ERR_NONICKNAMEGIVEN");
		return (res);
	}
	if (9 < input.args[0].size())
	{
		set_err_res(&res, input, "Nickname too long: must be 9 characters or fewer");
		return (res);
	}
	for (int i = 0;input.args[0][i] != '\0';i++)
	{
		if (!isalnum(input.args[0][i]) && !is_special_char(input.args[0][i]))
		{
			set_err_res(&res, input, "432 :ERR_ERRONEUSNICKNAME");
			return (res);
		}
	}
	if (db.getClient(input.client_fd))
		is_registed = true;

	std::string registerd_nick;
	for (int i = 0; db.getClient(i) != NULL; i++)
	{
		if (db.getClient(i)->getNickname() == input.args[0])
		{
			set_err_res(&res, input, "433 :ERR_NICKNAMEINUSE");
			return (res);
		}
	}
	res.is_success = true;
	res.should_send = false;
	res.reply = "";
	res.target_fds.resize(1);
	res.target_fds[0] = input.client_fd;
	return (res);
}

void	NickCommand::set_err_res(t_response *res,
						const t_parsed& input,
						std::string errmsg) const
{
	res->is_success = false;
	res->should_send = true;
	res->reply = ":ft.irc " + errmsg + "\r\n";
	res->target_fds.resize(1);
	res->target_fds[0] = input.client_fd;
}

bool NickCommand::is_special_char(char c) const
{
	if (c == '-' || c == '[' || c == ']'
		|| c == '\\' || c == '`' || c == '^'
		|| c == '{' || c == '}')
		return (true);
	return (false);
}
