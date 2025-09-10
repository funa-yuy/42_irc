#include <cassert>
#include <iostream>

#include "Command/PassCommand.hpp"

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
		std::vector<t_response> response_list = pass.execute(in, db);
		assert(response_list.size() == 1);
		const t_response & res = response_list[0];
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
		std::vector<t_response> response_list = pass.execute(in, db);
		assert(response_list.size() == 1);
		const t_response & res = response_list[0];
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
		std::vector<t_response> response_list = pass.execute(in, db);
		assert(response_list.size() == 1);
		const t_response & res = response_list[0];
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
		std::vector<t_response> response_list = pass.execute(in, db);
		assert(response_list.size() == 1);
		const t_response & res = response_list[0];
		assert(res.is_success == true);
		assert(res.should_send == false);
	}

	// createPassCommand関数が機能するか
	{
		Command* cmd = PassCommand::createPassCommand();
		int fd = 8;
		t_parsed in = makeInput("PASS", fd, std::vector<std::string>(1, "password"));
		db.addClient(fd);
		std::vector<t_response> response_list = cmd->execute(in, db);
		assert(response_list.size() == 1);
		const t_response & res = response_list[0];
		assert(res.is_success == true);
		assert(res.should_send == false);
	}
}

int main() {
	test_pass_command_all();
	std::cout << "PASS command tests: OK" << std::endl;
	return 0;
}

