#include "Command/JoinCommand.hpp"
#include <string>
#include <sstream>
#include <vector>

JoinCommand::JoinCommand() {}

JoinCommand::~JoinCommand() {}

Command*	JoinCommand::createJoinCommand() {
	return (new JoinCommand());
}

static std::vector<std::string> split(const std::string& str, char delimiter) {
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string token;

	while (std::getline(ss, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens;
}

static std::vector<s_join_item>	parse_join_args(const t_parsed& input) {
	std::vector<s_join_item>	res;
	std::vector<std::string> channels;
	std::vector<std::string> keys;

	if (input.args.size() > 0)
		channels = split(input.args[0], ',');
	if (input.args.size() > 1)
		keys = split(input.args[1], ',');

	for (size_t i = 0; i < channels.size(); i++)
	{
		s_join_item item;
		item.channel = channels[i];
		if (keys.size() > i)
			item.key = keys[i];
		res.push_back(item);
	}
	return (res);
}

static bool	is_validCmd(const t_parsed& input, t_response* res, Database& db, const s_join_item& item) {
	(void) input;
	(void) res;
	(void) db;
	(void) item;
	return(true);
}

static t_response	makeJoinBroadcast(const t_parsed& input, Database& db, Channel* channel) {
	t_response	res;
	const std::set<int>&	clientFds = channel->getClientFds();

	res.is_success = true;
	res.should_send = true;
	res.should_disconnect = false;
	res.reply = ":ft.irc " + db.getClient(input.client_fd)->getNickname() + " has joined " + channel->getName() + "\r\n";
	res.target_fds.assign(clientFds.begin(), clientFds.end());
	return (res);
}

static t_response	makeRplTopic(const t_parsed& input, Channel* channel) {
	t_response	res;

	res.is_success = true;
	res.should_send = true;
	res.should_disconnect = false;
	res.reply = ":ft.irc 332 Topic for " + channel->getName() + " : " + channel->getTopic() + "\r\n";
	res.target_fds.resize(1);
	res.target_fds[0] = input.client_fd;
	return (res);
}

static std::string	getNicknameList(Database& db, Channel* channel) {
	const std::set<int>& fds = channel->getClientFds();
	const int operatorFds = channel->getChannelOperatorFds();

	std::string names;
	if (operatorFds != -1) {
		names += "@";
		names += db.getClient(operatorFds)->getNickname();
	}
	for (std::set<int>::const_iterator it = fds.begin(); it != fds.end(); ++it) {
		if (*it == operatorFds)
			continue;
		if (!names.empty())
			names += " ";
		//if (hasVoice(channel, *it)) todo: ボイス権限があるユーザーの処理
			// names += "+";
		names += db.getClient(*it) ->getNickname();
	}
	return (names);
}

static t_response	makeRplNamreply(const t_parsed& input, Database& db, Channel* channel) {
	t_response	res;

	res.is_success = true;
	res.should_send = true;
	res.should_disconnect = false;
	// std::string status = getChannelStatus(channel);//todo: チャンネルのステータスを実装する
	res.reply = ":ft.irc 353 " + channel->getName() + " : " + getNicknameList(db, channel) + "\r\n";
	res.target_fds.resize(1);
	res.target_fds[0] = input.client_fd;
	return (res);
}

static const std::vector<t_response>	executeJoin(const t_parsed& input, Database& db, std::vector<s_join_item> items) {
	std::vector<t_response>	list;

	for (size_t i = 0; i < items.size(); i++)
	{
		t_response res;
		if (!is_validCmd(input, &res, db, items[i])) {
			list.push_back(res);
		} else {
			// update_database(input, db, items);//todo: 正常だった場合のデータ更新
			list.push_back(makeJoinBroadcast(input, db, db.getChannel(items[i].channel))); //JOIN成功メッセージ
			list.push_back(makeRplTopic(input, db.getChannel(items[i].channel))); //RPL_TOPIC
			list.push_back(makeRplNamreply(input, db, db.getChannel(items[i].channel))); //RPL_NAMREPLY
		}
	}
	return (list);
}

std::vector<t_response>	JoinCommand::execute(const t_parsed& input, Database& db) const {
	std::vector<t_response> response_list;
	t_response res;

	if (input.args[0] == "0")
	{
		// todo: すべてのチャンネルから退出する処理とレスポンス
		return (response_list);
	}

	std::vector<s_join_item> items = parse_join_args(input);
	response_list = executeJoin(input, db, items);
	return (response_list);
}
