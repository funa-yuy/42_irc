#include "Command/NickCommand.hpp"
#include "irc.hpp"

NickCommand::NickCommand() {}

NickCommand::~NickCommand() {}

const t_response	NickCommand::execute(const t_parsed& input, Database& db) const {

	t_response	res;

	if (is_validCmd(input, &res, db) == false)
		return (res);

	if (db.getClient(input.client_fd))
	{
		db.getClient(input.client_fd)->setNickname(input.args[0]);
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

bool	NickCommand::is_validCmd(const t_parsed& input, t_response* res, Database& db) const
{
	if (input.args.size() == 0 || input.args[0].size() < 1)
	{
		set_err_res(res, input, "431 :ERR_NONICKNAMEGIVEN");
		return (false);
	}
	if (9 < input.args[0].size())
	{
		set_err_res(res, input, "Nickname too long: must be 9 characters or fewer");
		return (false);
	}
	for (int i = 0;input.args[0][i] != '\0';i++)
	{
		if (!isalnum(input.args[0][i]) && !is_special_char(input.args[0][i]))
		{
			set_err_res(res, input, "432 :ERR_ERRONEUSNICKNAME");
			return (false);
		}
	}

	std::map<int, Client>::iterator it = db.getAllClient().begin();
	while (it != db.getAllClient().end())
	{
		if (it->second.getNickname() == input.args[0])
		{
			set_err_res(res, input, "433 :ERR_NICKNAMEINUSE");
			return (false);
		}
		it++;
	}
	return (true);
}

Command*	NickCommand::createNickCommand() {
	return (new NickCommand());
}
