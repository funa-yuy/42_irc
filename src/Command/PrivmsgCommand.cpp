#include "Command/PrivmsgCommand.hpp"

PrivmsgCommand::PrivmsgCommand() {}

PrivmsgCommand::~PrivmsgCommand() {}

Command*	PrivmsgCommand::createPrivmsgCommand() {
	return (new PrivmsgCommand());
}

/*
PRIVMSG <msgtarget> <text to be sent>
# 例
PRIVMSG Angel :yes I'm receiving it !

1. 個人間メッセージを送信できるようにする
2. チャンネルへの一斉送信をできるようにする
3. チャンネルのモードを確認して、送信可能かを確認してからの送信をできるようにする
4. 複雑な宛先指定で送信できるようにする
5. プレフィックス付きだった場合の送信をできるようにする
	- サーバー(サーバー名)マスク`$ mask`
	- ホストマスク`#mask`
*/

static	std::vector<int>	get_fd_ByNickname(std::string	msgtarget, Database& db) {
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

std::vector<int> get_fd_ByChannel(std::string	msgtarget, Database& db)
{
	std::vector<int>	fds;
	/*
		チャンネル名からチャンネル構造体を掴む
		チャンネルに所属しているクライアントを全部引っ張る
		fdsに打ち込む
		返す
	*/
	std::string channnel_name;

	std::vector<Client *>	clients;
	clients = db.getChannel(msgtarget)->getClients();
	return (fds);
}

static	std::vector<int>	get_target_fd(std::string target, Database& db) {
	// if (target.size() > 0 && target[0] == '#') //todo: チャンネルだった場合の処理
	// 	return (get_fd_ByChannel(target, db));

	return (get_fd_ByNickname(target, db));
}

static bool	is_validCmd(const t_parsed& input, t_response* res, Database& db) {

	// チャンネル名は以下で開始される[&, #, +, !]

	if (input.args.size() < 1)//ERR_NORECIPIENT 411 受信者が指定されていない
	{
		res->is_success = false;
		res->should_send = true;
		res->reply = ":ft.irc 411 :No recipient given PRIVMSG\r\n";
		res->target_fds.resize(1);
		res->target_fds[0] = input.client_fd;
		return(false);
	}
	else if (input.args.size() < 2)//ERR_NOTEXTTOSEND 412 送信テキストがない
	{
		res->is_success = false;
		res->should_send = true;
		res->reply = ":ft.irc 412 :No text to send\r\n";
		res->target_fds.resize(1);
		res->target_fds[0] = input.client_fd;
		return(false);
	}
	else if (get_fd_ByNickname(input.args[0], db).empty())//ERR_NOSUCHNICK 401 指定されたニックネーム/チャンネルがない
	{
		res->is_success = false;
		res->should_send = true;
		res->reply = ":ft.irc 401 " + input.args[0] + " :No such nick/channel\r\n";
		res->target_fds.resize(1);
		res->target_fds[0] = input.client_fd;
		return(false);
	}
	return(true);
}

const t_response	PrivmsgCommand::execute(const t_parsed& input, Database& db) const {
	t_response	res;

	if (!is_validCmd(input, &res, db))
		return (res);

	res.is_success = true;
	res.should_send = true;
	res.reply = input.args[1];
	res.target_fds = get_target_fd(input.args[0], db);

	return (res);
}
