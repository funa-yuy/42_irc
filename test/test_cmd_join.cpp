#include <cassert>
#include <iostream>
#include <algorithm>

#include "Database.hpp"
#include "Command/JoinCommand.hpp"

static t_parsed makeInput(const std::string& cmd, int fd, const std::vector<std::string>& args) {
	t_parsed in;
	in.cmd = cmd;
	in.client_fd = fd;
	in.args = args;
	return in;
}

static void test_success() {
	Database db("password");
	JoinCommand join;

	// 正常: 引数にチャンネルのみ
	{
		int	fd = 4;
		Client* cl4 = db.addClient(fd);
		cl4->setNickname("nick4");

		std::vector<std::string> args;
		args.push_back("#hoge");

		t_parsed in = makeInput("JOIN", fd, args);
		std::vector<t_response> res = join.execute(in, db);

		assert(res.size() == 3);
		// JOIN
		assert(res[0].is_success == true);
		assert(res[0].should_send == true);
		assert(res[0].should_disconnect == false);
		assert(res[0].reply.find("nick4 has joined #hoge") != std::string::npos);
		assert(res[0].target_fds.size() == 1 && res[0].target_fds[0] == fd);
		// 332 RPL_TOPIC
		assert(res[1].is_success == true);
		assert(res[1].should_send == true);
		assert(res[1].should_disconnect == false);
		assert(res[1].reply.find(" 332 Topic for #hoge :") != std::string::npos);
		assert(res[1].target_fds.size() == 1 && res[1].target_fds[0] == fd);
		// 353 RPL_NAMREPLY
		assert(res[2].is_success == true);
		assert(res[2].should_send == true);
		assert(res[2].should_disconnect == false);
		assert(res[2].reply.find(" 353 =#hoge ") != std::string::npos);
		assert(res[2].target_fds.size() == 1 && res[2].target_fds[0] == fd);
	}

	// 正常: 既存メンバーがいるチャンネルにJOIN → JOIN通知のfdsが全員分含む
	{
		int existing_fd = 4; // 上のケースで #hoge にいる既存メンバー
		int joiner_fd = 40;
		Client* cl40 = db.addClient(joiner_fd);
		cl40->setNickname("nick40");

		std::vector<std::string> args;
		args.push_back("#hoge");
		t_parsed in = makeInput("JOIN", joiner_fd, args);
		std::vector<t_response> res = join.execute(in, db);

		// 3通（JOIN, 332, 353）
		assert(res.size() == 3);
		// JOIN通知のfdsに既存(4)と新規(40)の両方が含まれているか
		const std::vector<int>& fds = res[0].target_fds;
		assert(fds.size() == 2);
		assert(std::find(fds.begin(), fds.end(), existing_fd) != fds.end());
		assert(std::find(fds.begin(), fds.end(), joiner_fd) != fds.end());
		assert(res[0].reply.find("nick40 has joined #hoge") != std::string::npos);
	}

	// 正常: 引数に複数のチャンネルと複数のキー
	{
		int	fd = 5;
		Client* cl5 = db.addClient(fd);
		cl5->setNickname("nick5");

		std::vector<std::string> args;
		args.push_back("&hoge,&fuga");
		args.push_back("hogeKey,fugaKey");

		t_parsed in = makeInput("JOIN", fd, args);
		std::vector<t_response> res = join.execute(in, db);

		assert(res.size() == 6);
		// 1つ目チャンネル: JOIN通知, 332, 353
		assert(res[0].reply.find("nick5 has joined &hoge") != std::string::npos);
		assert(res[0].target_fds.size() == 1 && res[0].target_fds[0] == fd);
		assert(res[1].reply.find(" 332 Topic for &hoge :") != std::string::npos);
		assert(res[1].target_fds.size() == 1 && res[1].target_fds[0] == fd);
		assert(res[2].reply.find(" 353 ") != std::string::npos);
		assert(res[2].target_fds.size() == 1 && res[2].target_fds[0] == fd);
		// 2つ目チャンネル: JOIN通知, 332, 353
		assert(res[3].reply.find("nick5 has joined &fuga") != std::string::npos);
		assert(res[3].target_fds.size() == 1 && res[3].target_fds[0] == fd);
		assert(res[4].reply.find(" 332 Topic for &fuga :") != std::string::npos);
		assert(res[4].target_fds.size() == 1 && res[4].target_fds[0] == fd);
		assert(res[5].reply.find(" 353 ") != std::string::npos);
		assert(res[5].target_fds.size() == 1 && res[5].target_fds[0] == fd);
	}

	// 正常: 引数に2つのチャンネルと1つのキー
	{
		int	fd = 6;
		Client* cl6 = db.addClient(fd);
		cl6->setNickname("nick6");

		std::vector<std::string> args;
		args.push_back("+hoge,+fuga");
		args.push_back("hogeKey");

		t_parsed in = makeInput("JOIN", fd, args);
		std::vector<t_response> res = join.execute(in, db);

		assert(res.size() == 6);
		// 1つ目チャンネル: JOIN通知, 332, 353
		assert(res[0].reply.find("nick6 has joined +hoge") != std::string::npos);
		assert(res[1].reply.find(" 332 Topic for +hoge :") != std::string::npos);
		assert(res[2].reply.find(" 353 ") != std::string::npos);
		// 2つ目チャンネル: JOIN通知, 332, 353
		assert(res[3].reply.find("nick6 has joined +fuga") != std::string::npos);
		assert(res[4].reply.find(" 332 Topic for +fuga :") != std::string::npos);
		assert(res[5].reply.find(" 353 ") != std::string::npos);
	}

	// 正常: 引数に1つのチャンネルと2つのキー(2つ目のキーは無視される)
	{
		int	fd = 7;
		Client* cl7 = db.addClient(fd);
		cl7->setNickname("nick7");

		std::vector<std::string> args;
		args.push_back("!hoge");
		args.push_back("hogeKey,fugaKey");

		t_parsed in = makeInput("JOIN", fd, args);
		std::vector<t_response> res = join.execute(in, db);

		assert(res.size() == 3);
		// JOIN
		assert(res[0].is_success == true);
		assert(res[0].should_send == true);
		assert(res[0].reply.find("nick7 has joined !hoge") != std::string::npos);
		assert(res[0].target_fds.size() == 1 && res[0].target_fds[0] == fd);
		// 332 RPL_TOPIC
		assert(res[1].is_success == true);
		assert(res[1].should_send == true);
		assert(res[1].reply.find(" 332 Topic for !hoge :") != std::string::npos);
		assert(res[1].target_fds.size() == 1 && res[1].target_fds[0] == fd);
		// 353 RPL_NAMREPLY
		assert(res[2].is_success == true);
		assert(res[2].should_send == true);
		assert(res[2].reply.find(" 353 ") != std::string::npos);
		assert(res[2].target_fds.size() == 1 && res[2].target_fds[0] == fd);
	}
}

int main() {
	test_success();//正常
	std::cout << "JOIN command tests: OK" << std::endl;
	return 0;
}

