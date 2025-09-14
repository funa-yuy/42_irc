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

std::vector<s_join_item> JoinCommand::parse_join_args(const t_parsed& input) const {
	std::vector<s_join_item> res;
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

static bool isAllowedChanstringChar(char c)
{
	if (c == 0x00 || c == 0x07 || c == 0x0A || c == 0x0D || c == 0x20 || c == 0x2C || c == 0x3A)
		return (false);
	return (true);
}

static bool isValidChanstring(const std::string& s)
{
	if (s.empty())
		return (false);

	bool seenColon = false;
	size_t lenFirst = 0;
	size_t lenSecond = 0;
	for (size_t i = 0; i < s.size(); ++i)
	{
		if (s[i] == ':')
		{
			if (seenColon)
				return (false);
			seenColon = true;
			if (lenFirst == 0)
				return (false);
			continue;
		}
		if (!isAllowedChanstringChar(s[i]))
	return (false);
		if (!seenColon)
			++lenFirst;
		else
			++lenSecond;
	}

	if (seenColon && lenSecond == 0)
		return (false);
	return (true);
}

bool	JoinCommand::isValidChannelName(const s_join_item& item) const {
	std::string channel = item.channel;
	if (channel.empty() || channel.size() < 2 || channel.size() > 50)
		return (false);

	if (!(channel[0] == '#' || channel[0] == '+' || channel[0] == '&' || channel[0] == '!'))
		return (false);

	if (channel[0] != '!') {
		std::string rest = channel.substr(1);
		if (!isValidChanstring(rest))
			return (false);
	}
	return (true);
}

static bool isValidChannelId(const std::string& id)
{
	if (id.size() != 5)
		return (false);
	for (size_t i = 0; i < id.size(); ++i)
	{
		if (!('A' <= id[i] && id[i] <= 'Z') && !('0' <= id[i] && id[i] <= '9'))
			return (false);
	}
	return (true);
}

bool	JoinCommand::isValidChanMask(const s_join_item& item) const {
	std::string channel = item.channel;
	if (channel.empty())
		return (false);

	if (channel[0] == '!') {
		std::string id = channel.substr(1, 5);
		if (!isValidChannelId(id))
			return (false);

		std::string rest = channel.substr(6);
		if (!isValidChanstring(rest))
			return (false);
	}
	return (true);
}

bool	JoinCommand::isValidParamsSize(const t_parsed& input, t_response* res) const {
	if (input.args.size() < 1)//ERR_NEEDMOREPARAMS 461 引数が無効
	{
		res->is_success = false;
		res->should_send = true;
		res->should_disconnect = false;
		res->reply = ":ft.irc 461 JOIN :Not enough parameters\r\n";
		res->target_fds.resize(1);
		res->target_fds[0] = input.client_fd;
		return(false);
	}
	return (true);
}

bool JoinCommand::is_validCmd(const t_parsed& input, t_response* res, Database& db, const s_join_item& item) const {
	(void) db;
	std::string	command = "JOIN";

	if (!isValidChannelName(item))//ERR_NOSUCHCHANNEL 403 指定されたチャネル名が無効である
	{
		res->is_success = false;
		res->should_send = true;
		res->should_disconnect = false;
		res->reply = ":ft.irc 403 " + item.channel + " :No such channel\r\n";
		res->target_fds.resize(1);
		res->target_fds[0] = input.client_fd;
		return(false);
	}
	if (!isValidChanMask(item))//ERR_BADCHANMASK 476 !で始まるチャンネル名が英数5文字 + 1文字以上の名前を満たさない
	{
		res->is_success = false;
		res->should_send = true;
		res->should_disconnect = false;
		res->reply = ":ft.irc 476 " + item.channel + " :Bad Channel Mask\r\n";
		res->target_fds.resize(1);
		res->target_fds[0] = input.client_fd;
		return(false);
	}
	return(true);
}

void	JoinCommand::updateDatabase(const t_parsed& input, Database& db, const s_join_item& item) const {
	std::string name = item.channel;
	Channel* ch = db.getChannel(name);
	if (ch == NULL) {
		Channel new_channel(name, input.client_fd);//todo: チャンネル名を大文字or小文字に正規化してから格納する
		db.addChannel(new_channel);
	} else {
		ch->addClientFd(input.client_fd);
	}
}

t_response	JoinCommand::makeJoinBroadcast(const t_parsed& input, Database& db, Channel* channel) const {
	t_response	res;
	const std::set<int>&	clientFds = channel->getClientFds();

	res.is_success = true;
	res.should_send = true;
	res.should_disconnect = false;
	res.reply = ":ft.irc " + db.getClient(input.client_fd)->getNickname() + " has joined " + channel->getName() + "\r\n";
	res.target_fds.assign(clientFds.begin(), clientFds.end());
	return (res);
}

t_response	JoinCommand::makeRplTopic(const t_parsed& input, Channel* channel) const {
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
	const std::set<int>& memberFds = channel->getClientFds();
	const std::set<int>& operatorFds = channel->getChannelOperatorFds();

	std::string names;
	for (std::set<int>::const_iterator it = operatorFds.begin(); it != operatorFds.end(); ++it) {
		if (!names.empty())
			names += " ";
		names += "@";
		names += db.getClient(*it)->getNickname();
	}
	for (std::set<int>::const_iterator it = memberFds.begin(); it != memberFds.end(); ++it) {
		if (operatorFds.find(*it) != operatorFds.end())
			continue;
		if (!names.empty())
			names += " ";
		names += db.getClient(*it)->getNickname();
	}
	return (names);
}

t_response	JoinCommand::makeRplNamreply(const t_parsed& input, Database& db, Channel* channel) const {
	t_response	res;

	res.is_success = true;
	res.should_send = true;
	res.should_disconnect = false;
	res.reply = ":ft.irc 353 =" + channel->getName() + " : " + getNicknameList(db, channel) + "\r\n";
	res.target_fds.resize(1);
	res.target_fds[0] = input.client_fd;
	return (res);
}

const std::vector<t_response> JoinCommand::executeJoin(const t_parsed& input, Database& db, std::vector<s_join_item> items) const {
	std::vector<t_response>	list;

	for (size_t i = 0; i < items.size(); i++)
	{
		t_response res;
		if (!is_validCmd(input, &res, db, items[i])) {
			list.push_back(res);
		} else {
			updateDatabase(input, db, items[i]);
			list.push_back(makeJoinBroadcast(input, db, db.getChannel(items[i].channel)));
			list.push_back(makeRplTopic(input, db.getChannel(items[i].channel)));
			list.push_back(makeRplNamreply(input, db, db.getChannel(items[i].channel)));
		}
	}
	return (list);
}

std::vector<t_response>	JoinCommand::execute(const t_parsed& input, Database& db) const {
	std::vector<t_response> response_list;
	t_response res;

	if (!isValidParamsSize(input, &res))
	{
		response_list.push_back(res);
		return (response_list);
	}

	if (input.args.size() > 0 && input.args[0] == "0")
	{
		// todo: すべてのチャンネルから退出する処理とレスポンス
		return (response_list);
	}

	std::vector<s_join_item> items = parse_join_args(input);

	response_list = executeJoin(input, db, items);
	return (response_list);
}
