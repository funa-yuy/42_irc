#include "Command/JoinCommand.hpp"
#include <string>
#include <sstream>
#include <vector>

JoinCommand::JoinCommand() {}

JoinCommand::~JoinCommand() {}

Command*	JoinCommand::createJoinCommand() {
	return (new JoinCommand());
}

struct s_join_item {
	std::string	channel;
	std::string	key;
};

/*
// ERR_NEEDMOREPARAMS(461)
// 	十分なパラメーターがない
//	461 <command>  :Not enough parameters
ERR_TOOMANYCHANNELS(405)
	許可されたチャネルの最大数に参加し、別のチャネルに参加しようとしている
	405 <channel name> :You have joined too many channels
// ERR_NOSUCHCHANNEL(403)
// 	指定されたチャネル名が無効である
// 	403 <channel name> :No such channel
ERR_CHANNELISFULL(471)
	471 <channel> :Cannot join channel (+l)
ERR_INVITEONLYCHAN(473)
	473 <channel> :Cannot join channel (+i)
ERR_BANNEDFROMCHAN(474)
	474 <channel> :Cannot join channel (+b)
ERR_BADCHANNELKEY(475)
	475 <channel> :Cannot join channel (+k)
ERR_BADCHANMASK(476)
	476 <channel> :Bad Channel Mask
ERR_TOOMANYTARGETS(407)
	指定された複雑な宛先指定(user@hostなど)が、複数のクライアントと一致した
	407 <target> :<error code> recipients. <abort message>
ERR_UNAVAILRESOURCE(437) ←多分実装する必要ない
	サーバーが現在ブロックされているチャネルに参加しようとしている
	437 <nick/channel> :Nick/channel is temporarily unavailable*/


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

static bool is_containsChar(const std::string& str, char target) {
	return (str.find(target) != std::string::npos);
}

static bool	is_validChannelName(std::vector<s_join_item> items, std::string* invalid_channelName) {
	for (size_t i = 0; i < items.size(); i++)
	{
		std::string channel = items[i].channel;
		if (channel.size() > 50) {
			*invalid_channelName = channel;
			return (false);
		}
		if (!(channel[0] == '&' || channel[0] == '#' || \
			channel[0] == '+' || channel[0] == '!')) {
			*invalid_channelName = channel;
			return (false);
			}
		if (is_containsChar(channel, ' ') || \
			is_containsChar(channel, 7) || \
			is_containsChar(channel, ',')) {
			*invalid_channelName = channel;
			return (false);
		}
	}
	return (true);
}

static bool	is_needMoreParams(const t_parsed& input, t_response* res) {
	std::string	command = "JOIN";

	if (input.args.size() < 1)//ERR_NEEDMOREPARAMS 461 引数が無効
	{
		res->is_success = false;
		res->should_send = true;
		res->should_disconnect = false;
		res->reply = ":ft.irc 461 " + command + " :Not enough parameters\r\n";
		res->target_fds.resize(1);
		res->target_fds[0] = input.client_fd;
		return(false);
	}
	return (true);
}

static bool	is_validCmd(const t_parsed& input, t_response* res, Database& db, std::vector<s_join_item> items) {
	// Client*		sender_client = db.getClient(input.client_fd);
	(void) db;
	std::string	command = "JOIN";
	std::string invalid_channelName;

	if (!is_validChannelName(items, &invalid_channelName))//ERR_NOSUCHCHANNEL 403 指定されたチャネル名が無効である
	{
		res->is_success = false;
		res->should_send = true;
		res->should_disconnect = false;
		res->reply = ":ft.irc 403 " + invalid_channelName + " :No such channel\r\n";
		res->target_fds.resize(1);
		res->target_fds[0] = input.client_fd;
		return(false);
	}
	return(true);
}

t_response	create_Join_response(const t_parsed& input, Database& db, Channel* channel) {
	t_response	res;
	const std::set<int>&	clientFds = channel->getClientFds();

	res.is_success = true;
	res.should_send = true;
	res.should_disconnect = false;
	res.reply = db.getClient(input.client_fd)->getNickname() + " has joined " + channel->getName() + "\r\n";
	res.target_fds.assign(clientFds.begin(), clientFds.end());
	return (res);
}

static const std::vector<t_response>	executeJoin(const t_parsed& input, Database& db, std::vector<s_join_item> items) {
	std::vector<t_response>	list;

	for (size_t i = 0; i < items.size(); i++)
	{
		t_response res;
		if (!is_validCmd(input, &res, db, items)) {//todo: items[i]にする
			//todo: 不正だった場合のresをpushする
			list.push_back(res);
		} else {
			//todo: 正常だった場合、データ更新と、resをpush
			// update_database(input, db, items);
			list.push_back(create_Join_response(input, db, db.getChannel(items[i].channel))); //JOIN成功メッセージ
			// create_rplTopic_response(); //RPL_TOPIC
			// create_rplNamreply_response(); //RPL_NAMREPLY
		}
	}
	return (list);
}


std::vector<t_response>	JoinCommand::execute(const t_parsed& input, Database& db) const {
	std::vector<t_response> response_list;
	t_response res;

	if (!is_needMoreParams(input, &res))
	{
		response_list.push_back(res);
		return (response_list);
	}

	if (input.args[0] == "0")
	{
		// todo: すべてのチャンネルから退出する処理とレスポンスをpush
		response_list.push_back(res);
		return (response_list);
	}

	std::vector<s_join_item> items = parse_join_args(input);

	std::cout << std::endl << "パース結果↓ " << std::endl;	//todo: デバック用
	for (size_t i = 0; i < items.size(); i++)
		std::cout << "channel: " << items[i].channel <<  " key: "  << items[i].key << std::endl;

	if (!is_validCmd(input, &res, db, items))
	{
		response_list.push_back(res);
		return (response_list);
	}

	// todo: 1個ずつ実行する関数
	response_list = executeJoin(input, db, items);
	return (response_list);
}
