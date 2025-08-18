#include "irc.hpp"
#include "Command/Command.hpp"
#include "Command/NickCommand.hpp"
#include <cassert>

int main()
{
	// db create
	Database db("pass");
	db.addClient(3);
	db.getClient(3)->setNickname("funa");

	t_response result;
	NickCommand nick;
	std::vector<std::string> args;
	std::vector<int> fds;
	t_parsed parsed;
	parsed.cmd = "NICK";

	// normal
	args.push_back("yohatana");
	parsed.args = args;
	parsed.client_fd = 3;
	parsed.msg = "";
	result = nick.execute(parsed, db);
	assert(result.is_success == true);
	assert(result.reply.empty() == true);
	assert(result.should_send == false);
	assert(result.target_fds[0] == 3);
	args.clear();
	fds.clear();

	args.push_back("yohatana");
	args.push_back("ken");
	parsed.args = args;
	parsed.client_fd = 3;
	parsed.msg = "";
	result = nick.execute(parsed, db);
	assert(result.is_success == true);
	assert(result.reply.empty() == true);
	assert(result.should_send == false);
	assert(result.target_fds[0] == 3);
	args.clear();
	fds.clear();

	// error
	// no args
	parsed.args = args;
	parsed.client_fd = 3;
	parsed.msg = "";
	result = nick.execute(parsed, db);
	assert(result.is_success == false);
	assert(result.reply == ":ft.irc 431 :ERR_NONICKNAMEGIVEN\r\n");
	assert(result.should_send == true);
	assert(result.target_fds[0] == 3);
	args.clear();
	fds.clear();

	// invalid arg
	args.push_back("y|hatana");
	parsed.args = args;
	parsed.client_fd = 3;
	parsed.msg = "";
	result = nick.execute(parsed, db);
	assert(result.is_success == false);
	assert(result.reply == ":ft.irc 432 :ERR_ERRONEUSNICKNAME\r\n");
	assert(result.should_send == true);
	assert(result.target_fds[0] == 3);
	args.clear();
	fds.clear();

	// 空文字の登録
	args.push_back("");
	parsed.args = args;
	parsed.client_fd = 3;
	parsed.msg = "";
	result = nick.execute(parsed, db);
	assert(result.is_success == false);
	assert(result.reply == ":ft.irc 431 :ERR_NONICKNAMEGIVEN\r\n");
	assert(result.should_send == true);
	assert(result.target_fds[0] == 3);
	args.clear();
	fds.clear();

	// すでに登録されているニックネーム
	// args.push_back("funa");
	// parsed.args = args;
	// parsed.client_fd = 3;
	// parsed.msg = "";
	// result = nick.execute(parsed, db);
	// assert(result.is_success == false);
	// assert(result.reply == ":ft.irc 433 :ERR_NICKNAMEINUSE\r\n");
	// assert(result.should_send == true);
	// assert(result.target_fds[0] == 3);
	// args.clear();
	// fds.clear();

	// 以下はサーバー間接続に使用するため不要
	// 436ERR_NICKCOLLISION:
	// サーバーがニックネームの衝突を検出した
	//（別のサーバーによって既に存在するニックネームが登録された）場合

	return (0);
}
