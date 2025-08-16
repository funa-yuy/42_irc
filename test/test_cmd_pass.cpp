#include <cassert>
#include <iostream>

#include "PassCommand.hpp"

static t_parsed makeInput(const std::string& cmd, int fd, const std::vector<std::string>& args) {
	t_parsed in;
	in.cmd = cmd;
	in.client_fd = fd;
	in.args = args;
	return in;
}

static void test_pass_command_all() {
	Database db("password");
	PassCommand pass;

	// エラー：　argsが0個 → ERR_NEEDMOREPARAMS 461
	{
		int fd = 4;
		t_parsed in = makeInput("PASS", fd, std::vector<std::string>());
		db.addClient(fd);
		t_response res = pass.execute(in, db);
		assert(res.is_success == false);
		assert(res.should_send == true);
		assert(res.reply.find("461") != std::string::npos);
		assert(res.target_fds.size() == 1 && res.target_fds[0] == fd);
	}

	// エラー：　既に登録済み → ERR_ALREADYREGISTRED 462
	{
		int fd = 5;
		t_parsed in = makeInput("PASS", fd, std::vector<std::string>(1, "password"));
		Client* c = db.addClient(fd);
		c->setIsRegistered(true);//isRegisteredをtrueにする
		t_response res = pass.execute(in, db);
		assert(res.is_success == false);
		assert(res.should_send == true);
		assert(res.reply.find("462") != std::string::npos);
		assert(res.target_fds.size() == 1 && res.target_fds[0] == fd);
	}

	// エラー：　パスワード不一致 → ERR_PASSWDMISMATCH 464
	{
		int fd = 6;
		t_parsed in = makeInput("PASS", fd, std::vector<std::string>(1, "wrong"));//間違ったパスワードを指定
		db.addClient(fd);
		t_response res = pass.execute(in, db);
		assert(res.is_success == false);
		assert(res.should_send == true);
		assert(res.reply.find("464") != std::string::npos);
		assert(res.target_fds.size() == 1 && res.target_fds[0] == fd);
	}

	// 正常: 正しいパスワード
	{
		int fd = 7;
		t_parsed in = makeInput("PASS", fd, std::vector<std::string>(1, "password"));
		db.addClient(fd);
		t_response res = pass.execute(in, db);
		assert(res.is_success == true);
		assert(res.should_send == false);
	}
}

int main() {
	test_pass_command_all();
	std::cout << "PASS command tests: OK" << std::endl;
	return 0;
}

