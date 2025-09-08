#include <cassert>
#include <iostream>

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

	// 正常: 正しい チャンネルのみ
	{
		int	fd = 4;
		db.addClient(fd);

		std::vector<std::string> args;
		args.push_back("#hoge");

		t_parsed in = makeInput("JOIN", fd, args);
		t_response res = join.execute(in, db);

		// std::string expected = ":funa!funauser@ft.irc JOIN funa :Send message successfully 1.\r\n";
		// assert(res.is_success == true);
		// assert(res.should_send == true);
		// assert(res.reply == expected);
		// assert(res.target_fds.size() == 1 && res.target_fds[0] == fd);
	}

	// 正常: 正しい 複数のチャンネルと複数のキー
	{
		int	fd = 5;
		db.addClient(fd);

		std::vector<std::string> args;
		args.push_back("&hoge,&fuga");
		args.push_back("hogeKey,fugaKey");

		t_parsed in = makeInput("JOIN", fd, args);
		t_response res = join.execute(in, db);

		// std::string expected = ":funa!funauser@ft.irc JOIN funa :Send message successfully 1.\r\n";
		// assert(res.is_success == true);
		// assert(res.should_send == true);
		// assert(res.reply == expected);
		// assert(res.target_fds.size() == 1 && res.target_fds[0] == fd);
	}

	// 正常: 正しい 2つのチャンネルと1つのキー
	{
		int	fd = 6;
		db.addClient(fd);

		std::vector<std::string> args;
		args.push_back("+hoge,+fuga");
		args.push_back("hogeKey");

		t_parsed in = makeInput("JOIN", fd, args);
		t_response res = join.execute(in, db);

		// std::string expected = ":funa!funauser@ft.irc JOIN funa :Send message successfully 1.\r\n";
		// assert(res.is_success == true);
		// assert(res.should_send == true);
		// assert(res.reply == expected);
		// assert(res.target_fds.size() == 1 && res.target_fds[0] == fd);
	}

	// 正常: 正しい 1つのチャンネルと2つのキー(2つ目のキーは無視される)
	{
		int	fd = 7;
		db.addClient(fd);

		std::vector<std::string> args;
		args.push_back("!hoge,");
		args.push_back("hogeKey,fugaKey");

		t_parsed in = makeInput("JOIN", fd, args);
		t_response res = join.execute(in, db);

		// std::string expected = ":funa!funauser@ft.irc JOIN funa :Send message successfully 1.\r\n";
		// assert(res.is_success == true);
		// assert(res.should_send == true);
		// assert(res.reply == expected);
		// assert(res.target_fds.size() == 1 && res.target_fds[0] == fd);
	}
}

// static void test_factory() {
// 	Database db("password");
// 	Command* cmd = JoinCommand::createJoinCommand();
// 	int fd = 6;

// 	Client* c = db.addClient(fd);
// 	c->setNickname("ken");
// 	c->setUsername("kenuser");

// 	std::vector<std::string> args;
// 	args.push_back("ken");
// 	args.push_back("This is createJoinCommand.");

// 	t_parsed in = makeInput("JOIN", fd, args);
// 	t_response res = cmd->execute(in, db);

// 	std::string expected = ":ken!kenuser@ft.irc JOIN ken :This is createJoinCommand.\r\n";
// 	assert(res.is_success == true);
// 	assert(res.should_send == true);
// 	assert(res.reply == expected);
// 	assert(res.target_fds.size() == 1 && res.target_fds[0] == fd);
// }

static void test_err_461_needmoreparams() {
	Database db("password");
	JoinCommand join;

	int fd = 10;
	db.addClient(fd);

	std::vector<std::string> args; // 空
	t_parsed in = makeInput("JOIN", fd, args);
	t_response res = join.execute(in, db);

	assert(res.is_success == false);
	assert(res.should_send == true);
	assert(res.reply.find("461") != std::string::npos);
	assert(res.target_fds.size() == 1 && res.target_fds[0] == fd);
}

static void test_err_403_nosuchchannel() {
	Database db("password");
	JoinCommand join;

	{
		int fd = 11;
		db.addClient(fd);

		std::vector<std::string> args;
		args.push_back("foo"); // 無効: "#", "&", "+", "!" で開始していない
		t_parsed in = makeInput("JOIN", fd, args);
		t_response res = join.execute(in, db);
		assert(res.is_success == false);
		assert(res.should_send == true);
		assert(res.reply.find("403 foo") != std::string::npos);
		assert(res.target_fds.size() == 1 && res.target_fds[0] == fd);
	}

	{
		int fd = 12;
		db.addClient(fd);

		std::vector<std::string> args;
		args.push_back("f  oo"); // 無効: 空白が含まれている
		t_parsed in = makeInput("JOIN", fd, args);
		t_response res = join.execute(in, db);
		assert(res.is_success == false);
		assert(res.should_send == true);
		assert(res.reply.find("403 f  oo") != std::string::npos);
		assert(res.target_fds.size() == 1 && res.target_fds[0] == fd);
	}
}

int main() {
	test_success();//正常
	// test_factory();// JoinCommand関数が機能するか
	test_err_461_needmoreparams();// エラー: ERR_NEEDMOREPARAMS 461 引数が無い
	test_err_403_nosuchchannel();// エラー: ERR_NOSUCHCHANNEL 403 チャンネル名が不正
	std::cout << "JOIN command tests: OK" << std::endl;
	return 0;
}

