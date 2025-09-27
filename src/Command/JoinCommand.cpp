#include "Command/JoinCommand.hpp"

JoinCommand::JoinCommand() {}

JoinCommand::~JoinCommand() {}

Command*	JoinCommand::createJoinCommand() {
	return (new JoinCommand());
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

static void	set_err_res(t_response *res, const t_parsed& input, std::string errmsg) {
	res->is_success = false;
	res->should_send = true;
	res->should_disconnect = false;
	res->reply = ":ft.irc " + errmsg + "\r\n";
	res->target_fds.resize(1);
	res->target_fds[0] = input.client_fd;
}

bool	JoinCommand::isValidParamsSize(const t_parsed& input, t_response* res, Database& db) const {
	if (input.args.size() < 1)//ERR_NEEDMOREPARAMS 461 引数が無効
	{
		set_err_res(res, input, "461 " + db.getClient(input.client_fd)->getNickname() + " JOIN :Not enough parameters");
		return(false);
	}
	return (true);
}

bool JoinCommand::is_validCmd(const t_parsed& input, t_response* res, Database& db, const s_join_item& item) const {
	std::string nick = db.getClient(input.client_fd)->getNickname() ;

	if (!isValidChannelName(item))//ERR_NOSUCHCHANNEL 403 指定されたチャネル名が無効である
	{
		set_err_res(res, input, "403 " + nick + " " + item.channel + " :No such channel");
		return(false);
	}
	if (!isValidChanMask(item))//ERR_BADCHANMASK 476 !で始まるチャンネル名が英数5文字 + 1文字以上の名前を満たさない
	{
		set_err_res(res, input, "476 " + nick + " " + item.channel + " :Bad Channel Mask");
		return(false);
	}
	const Channel *c =  db.getChannel(item.channel);
	if (c == NULL)
		return (true);
	if (c->getHasLimit() && c->getClientFds().size() >= c->getLimit())//ERR_CHANNELISFULL 471 参加できるユーザー数を超えている
	{
		set_err_res(res, input, "471 " + nick + " " + item.channel + " :Cannot join channel (+l)");
		return(false);
	}
	if (c->getInviteOnly() && !c->isInvited(input.client_fd))//ERR_INVITEONLYCHAN 473 招待されていない
	{
		set_err_res(res, input, "473 " + nick + " " + item.channel + " :Cannot join channel (+i)");
		return(false);
	}
	if(c->getHasKey() && c->getKey() != item.key)//ERR_BADCHANNELKEY 475 keyが間違っている
	{
		set_err_res(res, input, "475 " + nick + " " + item.channel + " :Cannot join channel (+k)");
		return(false);
	}
	return(true);
}

void	JoinCommand::updateDatabase(const t_parsed& input, Database& db, const s_join_item& item) const {
	std::string name = item.channel;
	Channel* ch = db.getChannel(name);
	if (ch == NULL) {
		Channel new_channel(name, input.client_fd);
		db.addChannel(new_channel);
	} else {
		ch->addClientFd(input.client_fd);
	}
}

t_response	JoinCommand::makeJoinBroadcast(const t_parsed& input, Database& db, Channel* channel) const {
	t_response	res;
	const std::set<int>&	clientFds = channel->getClientFds();

	Client* client = db.getClient(input.client_fd);
	std::string nick = client->getNickname();
	std::string user = client->getUsername();
	std::string chanName = channel->getName();
	std::string source = ":" + nick + "!" + user + "@ft.irc";

	res.is_success = true;
	res.should_send = true;
	res.should_disconnect = false;
	res.reply = source + " JOIN " + chanName + "\r\n";
	res.target_fds.assign(clientFds.begin(), clientFds.end());
	return (res);
}

t_response	JoinCommand::makeRplTopic(const t_parsed& input, Database& db, Channel* channel) const {
	t_response	res;

	Client* client = db.getClient(input.client_fd);
	std::string nick = client->getNickname();
	std::string chanName = channel->getName();
	std::string topic = channel->getTopic();

	res.is_success = true;
	res.should_send = true;
	res.should_disconnect = false;
	res.reply = ":ft.irc 332 " + nick + " " + chanName + " :" + topic + "\r\n";
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
		if (channel->isOperator(*it))
			continue;
		if (!names.empty())
			names += " ";
		names += db.getClient(*it)->getNickname();
	}
	return (names);
}

t_response	JoinCommand::makeRplNamreply(const t_parsed& input, Database& db, Channel* channel) const {
	t_response	res;

	Client* client = db.getClient(input.client_fd);
	std::string nick = client->getNickname();
	std::string chanName = channel->getName();

	res.is_success = true;
	res.should_send = true;
	res.should_disconnect = false;
	res.reply = ":ft.irc 353 " +  nick + " = " + chanName + " :" + getNicknameList(db, channel) + "\r\n";
	res.target_fds.resize(1);
	res.target_fds[0] = input.client_fd;
	return (res);
}

t_response	JoinCommand::makeEndofnames(const t_parsed& input, Database& db, Channel* channel) const {
	t_response	res;

	Client* client = db.getClient(input.client_fd);
	std::string nick = client->getNickname();
	std::string chanName = channel->getName();

	res.is_success = true;
	res.should_send = true;
	res.should_disconnect = false;
	res.reply = ":ft.irc 366 " + nick + " " + chanName + " :End of /NAMES list\r\n";
	res.target_fds.resize(1);
	res.target_fds[0] = input.client_fd;
	return (res);
}

const std::vector<t_response> JoinCommand::leaveAllJoinedChannels(const t_parsed& input, Database& db) const {
	std::vector<t_response> response_list;
	std::vector<std::string> allNames = db.getAllChannelNames();

	for (size_t i = 0; i < allNames.size(); ++i)
	{
		Channel* ch = db.getChannel(allNames[i]);
		if (ch == NULL)
			continue;
		const std::set<int>& members = ch->getClientFds();
		if (members.find(input.client_fd) == members.end())
			continue;

		t_response res;
		res.is_success = true;
		res.should_send = true;
		res.should_disconnect = false;
		Client* client = db.getClient(input.client_fd);
		std::string source = ":" + client->getNickname() + "!" + client->getUsername() + "@ft.irc";
		res.reply = source + " PART " + ch->getName() + "\r\n";
		res.target_fds.assign(ch->getClientFds().begin(), ch->getClientFds().end());
		response_list.push_back(res);

		ch->removeClientFd(input.client_fd);
		if (ch->getClientFds().empty())
			db.removeChannel(allNames[i]);
	}
	return (response_list);
}

const std::vector<t_response> JoinCommand::executeJoin(const t_parsed& input, Database& db, std::vector<s_join_item> items) const {
	std::vector<t_response>	list;

	for (size_t i = 0; i < items.size(); i++)
	{
		t_response res;
		if (!is_validCmd(input, &res, db, items[i])) {
			list.push_back(res);
		} else {
			Channel* ch = db.getChannel(items[i].channel);
			if (ch != NULL && ch->isMember(input.client_fd))
				continue;
			updateDatabase(input, db, items[i]);
			ch = db.getChannel(items[i].channel);
			list.push_back(makeJoinBroadcast(input, db, ch));
			list.push_back(makeRplTopic(input, db, ch));
			list.push_back(makeRplNamreply(input, db, ch));
			list.push_back(makeEndofnames(input, db, ch));
		}
	}
	return (list);
}

std::vector<t_response>	JoinCommand::execute(const t_parsed& input, Database& db) const {
	std::vector<t_response> response_list;
	t_response res;

	if (!isValidParamsSize(input, &res, db))
	{
		response_list.push_back(res);
		return (response_list);
	}

	if (input.args.size() > 0 && (input.args[0] == "0" ||  input.args[0] == "#0"))
	{
		response_list = leaveAllJoinedChannels(input, db);
		set_err_res(&res, input, "403 " + db.getClient(input.client_fd)->getNickname() + " #0 :No such channel");
		response_list.push_back(res);
		return (response_list);
	}

	std::vector<s_join_item> items = parse_join_args(input);

	response_list = executeJoin(input, db, items);
	return (response_list);
}
