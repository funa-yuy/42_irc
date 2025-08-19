#include "Command/PassCommand.hpp"

PassCommand::PassCommand() {}

PassCommand::~PassCommand() {}

Command*	PassCommand::createPassCommand() {
	return (new PassCommand());
}

bool	is_validCmd(const t_parsed& input, t_response* res, Database& db) {
	Client*		sender_client = db.getClient(input.client_fd);

	if (input.args.size() < 1)//ERR_NEEDMOREPARAMS 461 引数が無効
	{
		res->is_success = false;
		res->should_send = true;
		res->reply = ":ft.irc 461 :Not enough parameters\r\n";
		res->target_fds.resize(1);
		res->target_fds[0] = input.client_fd;
		return(false);
	}
	else if (sender_client->getIsRegistered())//ERR_ALREADYREGISTRED 462 登録済みのクライアントが実行した
	{
		res->is_success = false;
		res->should_send = true;
		res->reply = ":ft.irc 462 :Already registered\r\n";
		res->target_fds.resize(1);
		res->target_fds[0] = input.client_fd;
		return(false);
	}
	else if (db.getPassword() != input.args[0])//ERR_PASSWDMISMATCH 464 パスワードが正しくない
	{
		res->is_success = false;
		res->should_send = true;
		res->reply = ":ft.irc 464 :Password incorrect\r\n";
		res->target_fds.resize(1);
		res->target_fds[0] = input.client_fd;
		return(false);
	}
	return(true);
}

const t_response	PassCommand::execute(const t_parsed& input, Database& db) const {
	t_response	res;

	if (!is_validCmd(input, &res, db))
		return (res);

	res.is_success = true;
	res.should_send = false;

	return (res);
}
