#include "Command/PrivmsgCommand.hpp"

PrivmsgCommand::PrivmsgCommand() {}

PrivmsgCommand::~PrivmsgCommand() {}

Command*	PrivmsgCommand::createPrivmsgCommand() {
	return (new PrivmsgCommand());
}

/*
PRIVMSG <msgtarget> <text to be sent>
# 例
PRIVMSG Angel :yes I'm receiving it ! ; Command to send a message to Angel.

1. 個人間メッセージをできるようにする
2. チャンネルへの一斉送信をできるようにする
3. チャンネルのモードを確認して、送信可能かを確認してからの送信をできるようにする
4. プレフィックス付きだった場合の送信をできるようにする
	- サーバー(サーバー名)マスク$ mask
	- ホストマスク#mask
*/

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

std::vector<int>	get_target_fds(std::string	msgtarget, Database& db) {
	std::vector<int>	res;

	std::map<int, Client> Clients = db.getAllClient();
	std::map<int, Client>::iterator it = Clients.begin();
	while (it != Clients.end())
	{
		if (it->second.getNickname() == msgtarget)
		{
			res.push_back(it->second.getFd());
			return (res);
		}
		it++;
	}
	return (res);
}

const t_response	PrivmsgCommand::execute(const t_parsed& input, Database& db) const {
	t_response	res;

	// if (!is_validCmd(input, &res, db))
	// 	return (res);

	res.is_success = true;
	res.should_send = true;
	res.reply = input.args[1];
	res.target_fds = get_target_fds(input.args[0], db);

	return (res);
}
