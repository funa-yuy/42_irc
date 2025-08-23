#include <cassert>
#include <iostream>

#include "Command/PrivmsgCommand.hpp"

static t_parsed makeInput(const std::string& cmd, int fd, const std::vector<std::string>& args) {
	t_parsed in;
	in.cmd = cmd;
	in.client_fd = fd;
	in.args = args;
	return in;
}

static void test_pass_command_all() {
	Database db("password");
	PrivmsgCommand privmsg;
	std::vector<std::string> args;

	// 正常: 正しい		送信者と宛先が、同じfd(同じ人)の場合
	{
		int	fd = 4;
		std::vector<std::string> args;
		args.push_back("funa");
		args.push_back("Send message successfully 1.");
		t_parsed in = makeInput("PRIVMSG", fd, args);
		Client* c = db.addClient(fd);
		c->setNickname("funa");
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

	// createPassCommand関数が機能するか
	{
		Command* cmd = PrivmsgCommand::createPrivmsgCommand();
		int fd = 6;
		std::vector<std::string> args;
		args.push_back("ken");
		args.push_back("This is createPrivmsgCommand.");
		t_parsed in = makeInput("PRIVMSG", fd, args);
		Client* c = db.addClient(fd);
		c->setNickname("ken");
		t_response res = cmd->execute(in, db);
		assert(res.is_success == true);
		assert(res.should_send == true);
		assert(res.reply == "This is createPrivmsgCommand.");
		assert(res.target_fds.size() == 1 && res.target_fds[0] == fd);
	}

	// エラー：　ERR_NORECIPIENT 411 受信者が指定されていない
	{
		int	fd = 7;
		std::vector<std::string> args;
		t_parsed in = makeInput("PRIVMSG", fd, args);
		t_response res = privmsg.execute(in, db);
		assert(res.is_success == false);
		assert(res.should_send == true);
		assert(res.reply.find("411") != std::string::npos);
		assert(res.target_fds.size() == 1 && res.target_fds[0] == fd);
	}

	// エラー：　ERR_NOTEXTTOSEND 412 送信テキストがない
	{
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

	// エラー：　ERR_NOSUCHNICK 401 指定されたニックネーム/チャンネルがない
	{
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
}

int main() {
	test_pass_command_all();
	std::cout << "PRIVMSG command tests: OK" << std::endl;
	return 0;
}

