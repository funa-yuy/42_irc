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
ERR_NEEDMOREPARAMS(461)
十分なパラメーターがない
461 <command>  :Not enough parameters
ERR_TOOMANYCHANNELS(405)
許可されたチャネルの最大数に参加し、別のチャネルに参加しようとしている
405 <channel name> :You have joined too many channels
ERR_NOSUCHCHANNEL(403)
指定されたチャネル名が無効である
403 <channel name> :No such channel
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

static bool	is_validCmd(const t_parsed& input, t_response* res, Database& db, std::vector<s_join_item> items) {
	Client*		sender_client = db.getClient(input.client_fd);
	std::string	command = "JOIN";

	if (input.args.size() < 1)//ERR_NEEDMOREPARAMS 461 引数が無効
	{
		res->is_success = false;
		res->should_send = true;
		res->reply = ":ft.irc 461 " + command + " :Not enough parameters\r\n";
		res->target_fds.resize(1);
		res->target_fds[0] = input.client_fd;
		return(false);
	}
	// if (input.args.size() < 1)//ERR_NOSUCHCHANNEL 403 指定されたチャネル名が無効である
	// {
	// 	res->is_success = false;
	// 	res->should_send = true;
	// 	res->reply = ":ft.irc 403 " + channel_name + " :No such channel\r\n";
	// 	res->target_fds.resize(1);
	// 	res->target_fds[0] = input.client_fd;
	// 	return(false);
	// }
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

	std::vector<std::string> channels = split(input.args[0], ',');
	std::vector<std::string> keys = split(input.args[1], ',');

	for (size_t i = 0; i < channels.size(); i++)
	{
		s_join_item item;
		item.channel = channels[i];
		item.key = keys[i];
		res.push_back(item);
	}
	return (res);
}

const t_response	JoinCommand::execute(const t_parsed& input, Database& db) const {
	t_response	res;
	Client *	sender_client = db.getClient(input.client_fd);

	std::vector<s_join_item> items = parse_join_args(input);

	if (!is_validCmd(input, &res, db, items))
		return (res);
	//todo: t_responseをセット
	// for (size_t i = 0; i < items.size(); i++)
	// {
	// 	std::cout << "channel: " << items[i].channel <<  " key: "  << items[i].key << std::endl;
	// }
	return (res);
}
