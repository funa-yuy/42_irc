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

static bool	is_validCmd(const t_parsed& input, t_response* res, Database& db, std::vector<s_join_item> items) {
	// Client*		sender_client = db.getClient(input.client_fd);
	(void) db;
	std::string	command = "JOIN";
	std::string invalid_channelName;

	if (input.args.size() < 1)//ERR_NEEDMOREPARAMS 461 引数が無効
	{
		res->is_success = false;
		res->should_send = true;
		res->reply = ":ft.irc 461 " + command + " :Not enough parameters\r\n";
		res->target_fds.resize(1);
		res->target_fds[0] = input.client_fd;
		return(false);
	}
	if (!is_validChannelName(items, &invalid_channelName))//ERR_NOSUCHCHANNEL 403 指定されたチャネル名が無効である
	{
		res->is_success = false;
		res->should_send = true;
		res->reply = ":ft.irc 403 " + invalid_channelName + " :No such channel\r\n";
		res->target_fds.resize(1);
		res->target_fds[0] = input.client_fd;
		return(false);
	}
	return(true);
}

std::vector<std::string> split(const std::string& str, char delimiter) {
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string token;

	while (std::getline(ss, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens;
}

std::vector<s_join_item>	parse_join_args(const t_parsed& input) {
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

const t_response	JoinCommand::execute(const t_parsed& input, Database& db) const {
	t_response	res;
	// Client *	sender_client = db.getClient(input.client_fd);

	std::vector<s_join_item> items = parse_join_args(input);
	//todo: デバック用
	std::cout << std::endl << "パース結果↓ " << std::endl;
	for (size_t i = 0; i < items.size(); i++)
		std::cout << "channel: " << items[i].channel <<  " key: "  << items[i].key << std::endl;

	//todo: 1個ずつ不正チェックと実行をする?
	// for (size_t i = 0; i < items.size(); i++)
	// {
	// 	t_response res;
	// 	if (is_validCmd(input, &res, db, items))
	// 		res = update_database(input, db, items);
	// 	response.push_back(res);
	// }

	//todo: t_responseをセット
	return (res);
}
