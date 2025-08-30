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

		std::vector<std::string> args;
		args.push_back("funa");
		args.push_back("Send message successfully 1.");

		t_parsed in = makeInput("PRIVMSG", fd, args);
		t_response res = privmsg.execute(in, db);
		assert(res.is_success == true);
		assert(res.should_send == true);
		assert(res.reply == "Send message successfully 1.");
		assert(res.target_fds.size() == 1 && res.target_fds[0] == fd);
	}

	// 正常: 正しい		送信者と宛先が、異なるfd(違う人)の場合
	{
		int	fd = 5;

		std::vector<std::string> args;
		args.push_back("funa");
		args.push_back("Send message successfully 2.");

		t_parsed in = makeInput("PRIVMSG", fd, args);
		t_response res = privmsg.execute(in, db);
		assert(res.is_success == true);
		assert(res.should_send == true);
		assert(res.reply == "Send message successfully 2.");
		assert(res.target_fds.size() == 1 && res.target_fds[0] == 4);
	}
}

static void test_factory() {
	Database db("password");
	Command* cmd = PrivmsgCommand::createPrivmsgCommand();
	int fd = 6;

	Client* c = db.addClient(fd);
	c->setNickname("ken");

	std::vector<std::string> args;
	args.push_back("ken");
	args.push_back("This is createPrivmsgCommand.");

	t_parsed in = makeInput("PRIVMSG", fd, args);
	t_response res = cmd->execute(in, db);
	assert(res.is_success == true);
	assert(res.should_send == true);
	assert(res.reply == "This is createPrivmsgCommand.");
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

// Database set_db(Database db)
// {
// 	// db.addChannel();
// 	return (db);
// }

// void test_channel_broadcast()
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

