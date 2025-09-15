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

bool PrivmsgCommand::is_channel(std::string target) const
{
	if (target.size() > 0 && (target[0] == '#' || target[0] == '&' || target[0] == '+' || target[0] == '!'))
		return (true);
	return (false);
}

std::vector<int>	PrivmsgCommand::get_fd_ByNickname(std::string	msgtarget, Database& db) const {
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

std::vector<int> PrivmsgCommand::get_fd_ByChannel(std::string	target, Database& db) const
{
	std::vector<int>	fds;
	std::string channelName;

	// チャンネル名は以下で開始される[&, #, +, !]
	// ！始まりの場合5文字の英数字 (A-Zまたは0-9) で構成

	// いらないかも
	// if (target[0] == '!' && target.size() == EXCLAMATION_CHANNEL_LEN)
	// {
	// 	for (int i = 1; i < EXCLAMATION_CHANNEL_LEN;i++)
	// 	{
	// 		if (!std::isalnum(target[i]))
	// 			return (fds);
	// 	}
	// }
	// else
	// 	return (fds);

	// target.erase(0, 1);
	if (db.getChannel(target) == NULL)
		return (fds);
	const std::set<int>& clients = db.getChannel(target)->getClientFds();
	std::set<int>::const_iterator it = clients.begin();
	while (it != clients.end())
	{
		fds.push_back(*it);
		it++;
	}
	return (fds);
}

bool PrivmsgCommand::is_belong_channel(const t_parsed& input, Database& db) const
{
	std::vector<int> fds =  get_fd_ByChannel(input.args[0], db);
	for (int i = 0; i < (int)fds.size();i++)
	{
		if (fds[i] == input.client_fd)
			return (true);
	}
	return (false);
}

std::vector<int>	PrivmsgCommand::get_target_fd(std::string target, Database& db) const{

	if (target.size() > 0 && is_channel(target))
		return (get_fd_ByChannel(target, db));
	return (get_fd_ByNickname(target, db));
}

bool	PrivmsgCommand::is_validCmd(const t_parsed& input, t_response* res, Database& db) const {

	if (input.args.size() < 1)//ERR_NORECIPIENT 411 受信者が指定されていない
		return (return_false_set_reply(res, input, " 411 :No recipient given PRIVMSG"));
	else if (input.args.size() < 2)//ERR_NOTEXTTOSEND 412 送信テキストがない
		return (return_false_set_reply(res, input, " 412 :No text to send"));
	else if (get_target_fd(input.args[0], db).empty()) //ERR_NOSUCHNICK 401 指定されたニックネーム/チャンネルがない
		return (return_false_set_reply(res, input, " 401 " + input.args[0] + " :No such nick/channel"));

	//チャンネルに参加していないとき
	if (is_channel(input.args[0]) && !is_belong_channel(input, db))
		return (return_false_set_reply(res, input, " 404 :You are not in this channel."));
	return(true);
}

std::vector<t_response>	PrivmsgCommand::execute(const t_parsed& input, Database& db) const {
	std::vector<t_response> response_list;
	t_response	res;

	res.is_success = false;
	res.should_send = false;
	res.should_disconnect = false;

	if (!is_validCmd(input, &res, db))
	{
		response_list.push_back(res);
		return (response_list);
	}

	Client * sender = db.getClient(input.client_fd);
	std::string	nick = sender->getNickname();
	std::string	user = sender->getUsername();

	std::string	target = input.args[0];
	std::string	text = input.args[1];

	std::vector<int>	fds = get_target_fd(target, db);

	std::string line =	":" + nick + "!" + user + "@" + SERVER_NAME + " PRIVMSG " + target + " :" + text + "\r\n";

	res.is_success = true;
	res.should_send = true;
	res.reply = line;
	res.target_fds = fds;

	response_list.push_back(res);
	return (response_list);
}

bool	PrivmsgCommand::return_false_set_reply(t_response *res,
												const t_parsed& input,
												std::string msg) const
{
	res->is_success = false;
	res->should_send = true;
	res->reply = SERVER_PREFIX + msg + "\r\n";
	res->target_fds.resize(1);
	res->target_fds[0] = input.client_fd;
	return (false);
}
