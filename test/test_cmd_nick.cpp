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

	std::vector<t_response> response_list;
	NickCommand nick;
	std::vector<std::string> args;
	std::vector<int> fds;
	t_parsed parsed;
	parsed.cmd = "NICK";

	// normal
	args.push_back("yohatana");
	parsed.args = args;
	parsed.client_fd = 3;
	response_list = nick.execute(parsed, db);
	assert(response_list.size() == 1);
	const t_response & res1 = response_list[0];
	assert(res1.is_success == true);
	assert(res1.reply.empty() == true);
	assert(res1.should_send == false);
	assert(res1.target_fds[0] == 3);
	args.clear();
	fds.clear();

	args.push_back("ken");
	args.push_back("nusu");
	parsed.args = args;
	parsed.client_fd = 3;
	response_list = nick.execute(parsed, db);
	assert(response_list.size() == 1);
	const t_response & res2 = response_list[0];
	assert(res2.is_success == true);
	assert(res2.reply.empty() == true);
	assert(res2.should_send == false);
	assert(res2.target_fds[0] == 3);
	args.clear();
	fds.clear();

	// error
	// no args
	parsed.args = args;
	parsed.client_fd = 3;
	response_list = nick.execute(parsed, db);
	assert(response_list.size() == 1);
	const t_response & res3 = response_list[0];
	assert(res3.is_success == false);
	assert(res3.reply == ":ft.irc 431 :must be input Nickname\r\n");
	assert(res3.should_send == true);
	assert(res3.target_fds[0] == 3);
	args.clear();
	fds.clear();

	// invalid arg
	args.push_back("y|hatana");
	parsed.args = args;
	parsed.client_fd = 3;
	response_list = nick.execute(parsed, db);
	assert(response_list.size() == 1);
	const t_response & res4 = response_list[0];
	assert(res4.is_success == false);
	assert(res4.reply == ":ft.irc 432 :Nickname includes disallowed character\r\n");
	assert(res4.should_send == true);
	assert(res4.target_fds[0] == 3);
	args.clear();
	fds.clear();

	// 空文字の登録
	args.push_back("");
	parsed.args = args;
	parsed.client_fd = 3;
	response_list = nick.execute(parsed, db);
	assert(response_list.size() == 1);
	const t_response & res5 = response_list[0];
	assert(res5.is_success == false);
	assert(res5.reply == ":ft.irc 431 :must be input Nickname\r\n");
	assert(res5.should_send == true);
	assert(res5.target_fds[0] == 3);
	args.clear();
	fds.clear();

	// すでに登録されているニックネーム
	args.push_back("ken");
	parsed.args = args;
	parsed.client_fd = 3;
	response_list = nick.execute(parsed, db);
	assert(response_list.size() == 1);
	const t_response & res6 = response_list[0];
	assert(res6.is_success == false);
	assert(res6.reply == ":ft.irc 433 :Nickname is already in use\r\n");
	assert(res6.should_send == true);
	assert(res6.target_fds[0] == 3);
	args.clear();
	fds.clear();

	// 文字数が9文字以上
	args.push_back("1234567890");
	parsed.args = args;
	parsed.client_fd = 3;
	response_list = nick.execute(parsed, db);
	assert(response_list.size() == 1);
	const t_response & res7 = response_list[0];
	assert(res7.is_success == false);
	assert(res7.reply == ":ft.irc 432 :Nickname too long, must be 9 characters or fewer\r\n");
	assert(res7.should_send == true);
	assert(res7.target_fds[0] == 3);
	args.clear();
	fds.clear();

	// 以下はサーバー間接続に使用するため不要
	// 436ERR_NICKCOLLISION:
	// サーバーがニックネームの衝突を検出した
	//（別のサーバーによって既に存在するニックネームが登録された）場合

	return (0);
}
