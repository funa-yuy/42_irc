#include <cassert>
#include <iostream>

#include "Command/UserCommand.hpp"

static t_parsed makeInput(const std::string& cmd, int fd, const std::vector<std::string>& args)
{
	t_parsed in;
	in.cmd = cmd;
	in.client_fd = fd;
	in.args = args;
	return in;
}

static void test_user_command_basic()
{
	Database db("password");
	UserCommand user;

	// 正常ケース：適切な4つの引数
	{
		int fd = 1;
		std::vector<std::string> args;
		args.push_back("testuser");  // username
		args.push_back("0");         // mode
		args.push_back("*");         // unused
		args.push_back("Real Name"); // realname

		t_parsed in = makeInput("USER", fd, args);
		Client* client = db.addClient(fd);
		client->setNickname("testnick");  // nickname設定済みと想定

		t_response res = user.execute(in, db);
		
		assert(res.is_success == true);
		assert(res.should_send == false);
		assert(client->getUsername() == "testuser");
		assert(client->getRealname() == "Real Name");
		assert(client->getUserReceived() == true);
	}
}

static void test_user_command_realname_with_colon()
{
	Database db("password");
	UserCommand user;

	// realname が ':' で始まる場合の処理テスト
	{
		int fd = 2;
		std::vector<std::string> args;
		args.push_back("testuser2");
		args.push_back("0");
		args.push_back("*");
		args.push_back(":Real Name With Colon");

		t_parsed in = makeInput("USER", fd, args);
		Client* client = db.addClient(fd);
		client->setNickname("testnick2");

		t_response res = user.execute(in, db);
		
		assert(res.is_success == true);
		assert(client->getRealname() == "Real Name With Colon");
	}
}

static void test_user_command_userlen_limit()
{
	Database db("password");
	UserCommand user;

	// USERLEN (10文字) 制限テスト
	{
		int fd = 3;
		std::vector<std::string> args;
		args.push_back("verylongusername");  // 16文字 -> 10文字に切り詰め
		args.push_back("0");
		args.push_back("*");
		args.push_back("Real Name");

		t_parsed in = makeInput("USER", fd, args);
		Client* client = db.addClient(fd);
		client->setNickname("testnick3");

		t_response res = user.execute(in, db);
		
		assert(res.is_success == true);
		assert(client->getUsername() == "verylongus");  // 10文字に切り詰められる
		assert(client->getUsername().length() == 10);
	}
}

static void test_user_command_errors()
{
	Database db("password");
	UserCommand user;

	// エラー：引数不足 (3つしかない) → ERR_NEEDMOREPARAMS 461
	{
		int fd = 4;
		std::vector<std::string> args;
		args.push_back("testuser");
		args.push_back("0");
		args.push_back("*");
		// 4つ目の引数(realname)がない

		t_parsed in = makeInput("USER", fd, args);
		Client* client = db.addClient(fd);
		client->setNickname("testnick4");

		t_response res = user.execute(in, db);
		
		assert(res.is_success == false);
		assert(res.should_send == true);
		assert(res.reply.find("461") != std::string::npos);
		assert(res.reply.find("Not enough parameters") != std::string::npos);
		assert(res.target_fds.size() == 1 && res.target_fds[0] == fd);
	}

	// エラー：引数が全くない
	{
		int fd = 5;
		std::vector<std::string> args;  // 空

		t_parsed in = makeInput("USER", fd, args);
		Client* client = db.addClient(fd);
		client->setNickname("testnick5");

		t_response res = user.execute(in, db);
		
		assert(res.is_success == false);
		assert(res.should_send == true);
		assert(res.reply.find("461") != std::string::npos);
	}

	// エラー：重複登録 → ERR_ALREADYREGISTRED 462
	{
		int fd = 6;
		std::vector<std::string> args;
		args.push_back("testuser6");
		args.push_back("0");
		args.push_back("*");
		args.push_back("Real Name");

		t_parsed in = makeInput("USER", fd, args);
		Client* client = db.addClient(fd);
		client->setNickname("testnick6");
		client->setUserReceived(true);  // 既にUSER受信済みに設定

		t_response res = user.execute(in, db);
		
		assert(res.is_success == false);
		assert(res.should_send == true);
		assert(res.reply.find("462") != std::string::npos);
		assert(res.reply.find("You may not reregister") != std::string::npos);
		assert(res.target_fds.size() == 1 && res.target_fds[0] == fd);
	}
}

static void test_user_command_edge_cases()
{
	Database db("password");
	UserCommand user;

	// エッジケース：存在しないクライアントfd
	{
		int fd = 999;  // 存在しないfd
		std::vector<std::string> args;
		args.push_back("testuser");
		args.push_back("0");
		args.push_back("*");
		args.push_back("Real Name");

		t_parsed in = makeInput("USER", fd, args);
		// db.addClient(fd) を呼ばない = 存在しないクライアント

		t_response res = user.execute(in, db);
		
		assert(res.is_success == false);
		assert(res.should_send == false);  // クライアントが存在しないので送信しない
	}

	// エッジケース：空のusername (USERLEN制限後)
	{
		int fd = 7;
		std::vector<std::string> args;
		args.push_back("");  // 空のusername
		args.push_back("0");
		args.push_back("*");
		args.push_back("Real Name");

		t_parsed in = makeInput("USER", fd, args);
		Client* client = db.addClient(fd);
		client->setNickname("testnick7");

		t_response res = user.execute(in, db);
		
		// 空のusernameでも技術的には処理される（RFC的にはグレーゾーン）
		assert(res.is_success == true);
		assert(client->getUsername() == "");
	}

	// エッジケース：空のrealname
	{
		int fd = 8;
		std::vector<std::string> args;
		args.push_back("testuser8");
		args.push_back("0");
		args.push_back("*");
		args.push_back("");  // 空のrealname

		t_parsed in = makeInput("USER", fd, args);
		Client* client = db.addClient(fd);
		client->setNickname("testnick8");

		t_response res = user.execute(in, db);
		
		assert(res.is_success == true);
		assert(client->getRealname() == "");
	}

	// エッジケース：realname が ":" のみ
	{
		int fd = 9;
		std::vector<std::string> args;
		args.push_back("testuser9");
		args.push_back("0");
		args.push_back("*");
		args.push_back(":");

		t_parsed in = makeInput("USER", fd, args);
		Client* client = db.addClient(fd);
		client->setNickname("testnick9");

		t_response res = user.execute(in, db);
		
		assert(res.is_success == true);
		assert(client->getRealname() == "");  // ":" が除去されて空になる
	}

	// エッジケース：ちょうどUSERLEN文字のusername
	{
		int fd = 10;
		std::vector<std::string> args;
		args.push_back("exactlyten");  // ちょうど10文字
		args.push_back("0");
		args.push_back("*");
		args.push_back("Real Name");

		t_parsed in = makeInput("USER", fd, args);
		Client* client = db.addClient(fd);
		client->setNickname("testnick10");

		t_response res = user.execute(in, db);
		
		assert(res.is_success == true);
		assert(client->getUsername() == "exactlyten");
		assert(client->getUsername().length() == 10);
	}
}

static void test_user_command_factory()
{
	// createUserCommand のテスト
	Command* cmd = UserCommand::createUserCommand();
	assert(cmd != NULL);
	
	// 動的確認：実際にUserCommandかどうか
	UserCommand* userCmd = dynamic_cast<UserCommand*>(cmd);
	assert(userCmd != NULL);
	
	delete cmd;
}

int main(void)
{
	test_user_command_basic();
	test_user_command_realname_with_colon();
	test_user_command_userlen_limit();
	test_user_command_errors();
	test_user_command_edge_cases();
	test_user_command_factory();
	
	return 0;
}
