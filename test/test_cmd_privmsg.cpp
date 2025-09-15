#include <cassert>
#include <iostream>

#include "Database.hpp"
#include "Command/PrivmsgCommand.hpp"

static t_parsed makeInput(const std::string& cmd, int fd, const std::vector<std::string>& args) {
	t_parsed in;
	in.cmd = cmd;
	in.client_fd = fd;
	in.args = args;
	return in;
}

static void test_success() {
	Database db("password");
	PrivmsgCommand privmsg;

	// 正常: 正しい		送信者と宛先が、同じfd(同じ人)の場合
	{
		int	fd = 4;

		Client* c = db.addClient(fd);
		c->setNickname("funa");
		c->setUsername("funauser");

		std::vector<std::string> args;
		args.push_back("funa");
		args.push_back("Send message successfully 1.");

		t_parsed in = makeInput("PRIVMSG", fd, args);
		std::vector<t_response> response_list = privmsg.execute(in, db);
		assert(response_list.size() == 1);
		const t_response & res = response_list[0];

		std::string expected = ":funa!funauser@ft.irc PRIVMSG funa :Send message successfully 1.\r\n";
		// ":" + nick + "!" + user + "@" + host + " PRIVMSG " + target + " :" + text + "\r\n"
		assert(res.is_success == true);
		assert(res.should_send == true);
		assert(res.reply == expected);
		assert(res.target_fds.size() == 1 && res.target_fds[0] == fd);
	}

	// 正常: 正しい		送信者と宛先が、異なるfd(違う人)の場合
	{
		int	sender_fd = 5;
		int	target_fd = 4;

		Client *	target = db.addClient(target_fd);
		target->setNickname("funa");
		target->setUsername("funauser");

		Client *	sender = db.addClient(sender_fd);
		sender->setNickname("user1");
		sender->setUsername("user1user");

		std::vector<std::string> args;
		args.push_back("funa");
		args.push_back("Send message successfully 2.");

		t_parsed in = makeInput("PRIVMSG", sender_fd, args);
		std::vector<t_response> response_list = privmsg.execute(in, db);
		assert(response_list.size() == 1);
		const t_response & res = response_list[0];

		std::string expected = ":user1!user1user@ft.irc PRIVMSG funa :Send message successfully 2.\r\n";
		assert(res.is_success == true);
		assert(res.should_send == true);
		assert(res.reply == expected);
		assert(res.target_fds.size() == 1 && res.target_fds[0] == 4);
	}
}

static void test_factory() {
	Database db("password");
	Command* cmd = PrivmsgCommand::createPrivmsgCommand();
	int fd = 6;

	Client* c = db.addClient(fd);
	c->setNickname("ken");
	c->setUsername("kenuser");

	std::vector<std::string> args;
	args.push_back("ken");
	args.push_back("This is createPrivmsgCommand.");

	t_parsed in = makeInput("PRIVMSG", fd, args);
	std::vector<t_response> response_list = cmd->execute(in, db);
	assert(response_list.size() == 1);
	const t_response & res = response_list[0];

	std::string expected = ":ken!kenuser@ft.irc PRIVMSG ken :This is createPrivmsgCommand.\r\n";
	assert(res.is_success == true);
	assert(res.should_send == true);
	assert(res.reply == expected);
	assert(res.target_fds.size() == 1 && res.target_fds[0] == fd);

	delete cmd;
}

static void test_err_411_norecipient() {
	Database db("password");
	PrivmsgCommand privmsg;
	int	fd = 7;

	std::vector<std::string> args;

	t_parsed in = makeInput("PRIVMSG", fd, args);
	std::vector<t_response> response_list = privmsg.execute(in, db);
	assert(response_list.size() == 1);
	const t_response & res = response_list[0];

	assert(res.is_success == false);
	assert(res.should_send == true);
	assert(res.reply.find("411") != std::string::npos);
	assert(res.target_fds.size() == 1 && res.target_fds[0] == fd);
}

static void test_err_412_notexttosend() {
	Database db("password");
	PrivmsgCommand privmsg;
	int	fd = 8;

	std::vector<std::string> args;
	args.push_back("nusu");

	t_parsed in = makeInput("PRIVMSG", fd, args);
	std::vector<t_response> response_list = privmsg.execute(in, db);
	assert(response_list.size() == 1);
	const t_response & res = response_list[0];

	assert(res.is_success == false);
	assert(res.should_send == true);
	assert(res.reply.find("412") != std::string::npos);
	assert(res.target_fds.size() == 1 && res.target_fds[0] == fd);
}


static void test_err_401_nosuchnick() {
	Database db("password");
	PrivmsgCommand privmsg;
	int	fd = 9;

	std::vector<std::string> args;
	args.push_back("Ghost");
	args.push_back("This is ERR_NOTEXTTOSEND 401.");

	t_parsed in = makeInput("PRIVMSG", fd, args);
	std::vector<t_response> response_list = privmsg.execute(in, db);
	assert(response_list.size() == 1);
	const t_response & res = response_list[0];

	assert(res.is_success == false);
	assert(res.should_send == true);
	assert(res.reply.find("401") != std::string::npos);
	assert(res.target_fds.size() == 1 && res.target_fds[0] == fd);
}

// /*

// チャンネル参加者へのブロードキャスト
static Database *set_db_test_channel_broadcast(Database *db)
{
	Channel channel1;
	Channel channel2;
	Channel channel3;
	std::map<int, Client> db_clients;

	// channel
	channel1.setName("#test1");
	channel1.setTopic("chanel1");
	channel1.addClientFd(3);

	channel2.setName("#test2");
	channel2.setTopic("chanel2");
	channel2.addClientFd(3);
	channel2.addClientFd(4);

	channel3.setName("#test3");
	channel3.setTopic("chanel3");
	channel3.addClientFd(3);
	channel3.addClientFd(4);
	channel3.addClientFd(5);

	db->addChannel(channel1);
	db->addChannel(channel2);
	db->addChannel(channel3);

	// client
	db->addClient(3);
	db->addClient(4);
	db->addClient(5);
	db->getClient(3)->setNickname("client1");
	db->getClient(4)->setNickname("client2");
	db->getClient(5)->setNickname("client3");
	return (db);
}

static void test_channel_broadcast()
{
	// db作成
	Database *db = new Database("pass");
	db = set_db_test_channel_broadcast(db);

	t_parsed parse;
	std::string target;
	std::string msg;
	std::vector<t_response> result;
	PrivmsgCommand privmsg;

	// チャンネル参加者が一人（送信者のみ）の場合
	target = "#test1";
	msg = "only one";
	parse.cmd = "PRIVMSG";
	parse.client_fd = 3;
	parse.args.push_back(target);
	parse.args.push_back(msg);
	result = privmsg.execute(parse, *db);

	assert(result.size() == 1);
	assert(result[0].is_success == true);
	assert(result[0].reply == ":client1!@ft.irc PRIVMSG #test1 :only one\r\n");
	assert(result[0].should_disconnect == false);
	assert(result[0].should_send == true);
	assert(result[0].target_fds.size() == 1);
	assert(result[0].target_fds.size() == 1);
	assert(result[0].target_fds[0] == parse.client_fd);
	result.clear();
	parse.args.clear();

	// チャンネル参加者が二人
	target = "#test2";
	msg = "member 2";
	parse.cmd = "PRIVMSG";
	parse.client_fd = 3;
	parse.args.push_back(target);
	parse.args.push_back(msg);
	result = privmsg.execute(parse, *db);

	assert(result.size() == 1);
	assert(result[0].is_success == true);
	assert(result[0].reply == ":client1!@ft.irc PRIVMSG #test2 :member 2\r\n");
	assert(result[0].should_disconnect == false);
	assert(result[0].should_send == true);
	assert(result[0].target_fds.size() == 2);
	assert(result[0].target_fds[0] == parse.client_fd);
	assert(result[0].target_fds[1] == 4);
	result.clear();
	parse.args.clear();

	// チャンネル参加者が三人
	target = "#test3";
	msg = "member 3 full!";
	parse.cmd = "PRIVMSG";
	parse.client_fd = 3;
	parse.args.push_back(target);
	parse.args.push_back(msg);
	result = privmsg.execute(parse, *db);

	assert(result.size() == 1);
	assert(result[0].is_success == true);
	assert(result[0].reply == ":client1!@ft.irc PRIVMSG #test3 :member 3 full!\r\n");
	assert(result[0].should_disconnect == false);
	assert(result[0].should_send == true);
	assert(result[0].target_fds.size() == 3);
	assert(result[0].target_fds[0] == parse.client_fd);
	assert(result[0].target_fds[1] == 4);
	assert(result[0].target_fds[2] == 5);
	result.clear();
	parse.args.clear();

	// エラー
	// 送信者がチャンネルに参加していない
	target = "#test1";
	msg = ":hello world";
	parse.cmd = "PRIVMSG";
	parse.client_fd = 10;
	parse.args.push_back(target);
	parse.args.push_back(msg);
	result = privmsg.execute(parse, *db);

	assert(result.size() == 1);
	assert(result[0].is_success == false);
	assert(result[0].reply == ":ft.irc 404 :You are not in this channel.\r\n");
	assert(result[0].should_disconnect == false);
	assert(result[0].should_send == true);
	assert(result[0].target_fds.size() == 1);
	assert(result[0].target_fds[0] == 10);
	result.clear();
	parse.args.clear();

	// 送信先のチャンネルが存在しない
	target = "noexist";
	msg = ":hello world";
	parse.cmd = "PRIVMSG";
	parse.client_fd = 3;
	parse.args.push_back(target);
	parse.args.push_back(msg);
	result = privmsg.execute(parse, *db);

	assert(result.size() == 1);
	assert(result[0].is_success == false);
	// :ft.irc 401 :No such nick/channel\r\nではない？
	assert(result[0].reply == ":ft.irc 401 noexist :No such nick/channel\r\n");
	assert(result[0].should_disconnect == false);
	assert(result[0].should_send == true);
	assert(result[0].target_fds.size() == 1);
	assert(result[0].target_fds[0] == 3);
	result.clear();
	parse.args.clear();

	delete(db);
}

// */

int main() {
	test_success();//正常
	test_factory();// PrivmsgCommand関数が機能するか
	test_err_411_norecipient();// エラー：　ERR_NORECIPIENT 411 受信者が指定されていない
	test_err_412_notexttosend();// エラー：　ERR_NOTEXTTOSEND 412 送信テキストがない
	test_err_401_nosuchnick();// エラー：　ERR_NOSUCHNICK 401 指定されたニックネーム/チャンネルがない
	test_channel_broadcast(); // チャンネルに所属している全員にブロードキャストを行う
	std::cout << "PRIVMSG command tests: OK" << std::endl;
	return 0;
}

