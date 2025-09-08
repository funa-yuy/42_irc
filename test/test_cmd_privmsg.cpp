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
		t_response res = privmsg.execute(in, db);

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
		t_response res = privmsg.execute(in, db);

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
	t_response res = cmd->execute(in, db);

	std::string expected = ":ken!kenuser@ft.irc PRIVMSG ken :This is createPrivmsgCommand.\r\n";
	assert(res.is_success == true);
	assert(res.should_send == true);
	assert(res.reply == expected);
	assert(res.target_fds.size() == 1 && res.target_fds[0] == fd);
}

static void test_err_411_norecipient() {
	Database db("password");
	PrivmsgCommand privmsg;
	int	fd = 7;

	std::vector<std::string> args;

	t_parsed in = makeInput("PRIVMSG", fd, args);
	t_response res = privmsg.execute(in, db);
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
	t_response res = privmsg.execute(in, db);
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
	t_response res = privmsg.execute(in, db);
	assert(res.is_success == false);
	assert(res.should_send == true);
	assert(res.reply.find("401") != std::string::npos);
	assert(res.target_fds.size() == 1 && res.target_fds[0] == fd);
}

// static Database set_db(Database& db)
// {
// 	Channel channel;
// 	std::map<int, Client> db_clients;
// 	std::vector<Client *> clients;

// 	for (int i = 0; i < 3; i++)
// 	{
// 		db.addClient(i + 2);
// 		db.getClient(i + 2)->setNickname("aaa");
// 		db.getClient(i + 2)->setUsername("bbb");
// 		db.getClient(i + 2)->setRealname("ccc");

// 		db_clients[i + 2] = *(db.getClient(i + 2));
// 	}

// 	channel.setName("name");
// 	channel.setClients(clients);
// 	return (db);
// }

// static void test_channel_broadcast()
// {
// 	// db作成
// 	Database db("pass");
// 	set_db(db);
// 	// パースド作成
// }

int main() {
	test_success();//正常
	test_factory();// PrivmsgCommand関数が機能するか
	test_err_411_norecipient();// エラー：　ERR_NORECIPIENT 411 受信者が指定されていない
	test_err_412_notexttosend();// エラー：　ERR_NOTEXTTOSEND 412 送信テキストがない
	test_err_401_nosuchnick();// エラー：　ERR_NOSUCHNICK 401 指定されたニックネーム/チャンネルがない
	std::cout << "PRIVMSG command tests: OK" << std::endl;
	return 0;
}

