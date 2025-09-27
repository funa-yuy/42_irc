#include <cassert>
#include <iostream>

#include "Database.hpp"
#include "Command/KickCommand.hpp"

static t_parsed makeInput(const std::string& cmd, int fd, const std::vector<std::string>& args) {
	t_parsed in;
	in.cmd = cmd;
	in.client_fd = fd;
	in.args = args;
	return in;
}

static void test_success_single_target() {
	Database db("password");
	KickCommand kick;

	int op_fd = 1;
	Client* op = db.addClient(op_fd);
	op->setNickname("op1");
	op->setUsername("u1");

	int victim_fd = 2;
	Client* victim = db.addClient(victim_fd);
	victim->setNickname("vic2");
	victim->setUsername("u2");

	Channel ch("#room", op_fd);
	ch.addClientFd(victim_fd);
	db.addChannel(ch);

	std::vector<std::string> args;
	args.push_back("#room");
	args.push_back("vic2");
	args.push_back("bye");

	std::vector<t_response> res = kick.execute(makeInput("KICK", op_fd, args), db);
	assert(res.size() == 1);
	assert(res[0].is_success == true);
	assert(res[0].should_send == true);
	assert(res[0].reply.find(" KICK #room vic2 :bye") != std::string::npos);

	Channel* room = db.getChannel(std::string("#room"));
	assert(room != NULL);
	const std::set<int>& members = room->getClientFds();
	assert(members.find(victim_fd) == members.end());
}

static void test_success_multi_targets() {
	Database db("password");
	KickCommand kick;

	int op_fd = 10;
	Client* op = db.addClient(op_fd);
	op->setNickname("op10");
	op->setUsername("u10");

	int v1_fd = 11; db.addClient(v1_fd)->setNickname("v1");
	int v2_fd = 12; db.addClient(v2_fd)->setNickname("v2");

	Channel ch("#multi", op_fd);
	ch.addClientFd(v1_fd);
	ch.addClientFd(v2_fd);
	db.addChannel(ch);

	std::vector<std::string> args;
	args.push_back("#multi");
	args.push_back("v1,v2");

	std::vector<t_response> res = kick.execute(makeInput("KICK", op_fd, args), db);
	assert(res.size() == 2);
	assert(res[0].reply.find(" KICK #multi v1") != std::string::npos);
	assert(res[1].reply.find(" KICK #multi v2") != std::string::npos);

	Channel* room = db.getChannel(std::string("#multi"));
	assert(room != NULL);
	const std::set<int>& members = room->getClientFds();
	assert(members.find(v1_fd) == members.end());
	assert(members.find(v2_fd) == members.end());
}

static void test_err_461_needmoreparams() {
	Database db("password");
	KickCommand kick;
	int fd = 20;
	db.addClient(fd)->setNickname("u20");

	std::vector<std::string> args; // 引数なし
	std::vector<t_response> res = kick.execute(makeInput("KICK", fd, args), db);
	assert(res.size() == 1);
	assert(res[0].is_success == false);
	assert(res[0].should_send == true);
	assert(res[0].reply.find(" 461 ") != std::string::npos);
}

static void test_err_403_482_442_441() {
	Database db("password");
	KickCommand kick;

	int fd = 30;
	Client* user = db.addClient(fd);
	user->setNickname("u30");
	user->setUsername("uu");

	// 403: チャンネルなし
	{
		std::vector<std::string> args;
		args.push_back("#nope");
		args.push_back("someone");
		std::vector<t_response> res = kick.execute(makeInput("KICK", fd, args), db);
		assert(res.size() == 1);
		assert(res[0].reply.find(" 403 u30 #nope ") != std::string::npos);
	}

	// チャンネルを作成（fdがOP）だが被キック者は不在 → 441
	Channel ch("#x", fd);
	db.addChannel(ch);
	{
		std::vector<std::string> args;
		args.push_back("#x");
		args.push_back("ghost");
		std::vector<t_response> res = kick.execute(makeInput("KICK", fd, args), db);
		assert(res.size() == 1);
		assert(res[0].reply.find(" 441 u30 ghost #x ") != std::string::npos);
	}

	// チャンネルメンバーでないユーザーが実行→ 442
	int outsider_fd = 32;
	db.addClient(outsider_fd)->setNickname("u32");
	{
		std::vector<std::string> args;
		args.push_back("#x");
		args.push_back("u30");
		std::vector<t_response> res = kick.execute(makeInput("KICK", outsider_fd, args), db);
		assert(res.size() == 1);
		assert(res[0].reply.find(" 442 u32 #x ") != std::string::npos);
	}

	// OPでないユーザが実行 → 482
	int notop_fd = 31; db.addClient(notop_fd)->setNickname("u31");
	db.getChannel(std::string("#x"))->addClientFd(notop_fd);
	{
		std::vector<std::string> args;
		args.push_back("#x");
		args.push_back("u30");
		std::vector<t_response> res = kick.execute(makeInput("KICK", notop_fd, args), db);
		assert(res.size() == 1);
		assert(res[0].reply.find(" 482 u31 #x ") != std::string::npos);
	}

}

int main() {
	test_success_single_target();
	test_success_multi_targets();
	test_err_461_needmoreparams();
	test_err_403_482_442_441();
	std::cout << "KICK command tests: OK" << std::endl;
	return 0;
}
