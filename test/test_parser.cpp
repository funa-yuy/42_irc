# include "../includes/Server.hpp"
# include "../includes/Parser.hpp"

#include <cassert>
int main()
{
	Parser parser;
	t_parsed result;
	std::vector<std::string> expect_v;

	// test
	result = parser.exec("PRIVMSG B :こんにちは\r\n", 4);
	expect_v.push_back("B");
	assert(result.cmd == "PRIVMSG");
	assert(result.args[0] == expect_v[0]);
	assert(result.msg == "こんにちは");
	expect_v.clear();

	// no msg
	result = parser.exec("PASS hunter2\r\n", 4);
	expect_v.push_back("hunter2");
	assert(result.cmd == "PASS");
	assert(result.args[0] == expect_v[0]);
	assert(result.msg == "");
	expect_v.clear();

	// multi
	result = parser.exec("USER nusu 0 * :Nusu Realname\r\n", 4);
	expect_v.push_back("nusu");
	expect_v.push_back("0");
	expect_v.push_back("*");
	assert(result.cmd == "USER");
	for(int i = 0; i < (int)expect_v.size();i++)
		assert(result.args[i] == expect_v[i]);
	assert(result.msg == "Nusu Realname");
	expect_v.clear();

	result = parser.exec("MODE #chatroom +o user123\r\n", 4);
	expect_v.push_back("#chatroom");
	expect_v.push_back("+o");
	expect_v.push_back("user123");
	assert(result.cmd == "MODE");
	for(int i = 0; i < (int)expect_v.size();i++)
		assert(result.args[i] == expect_v[i]);
	assert(result.msg == "");
	expect_v.clear();

	// space only msg
	result = parser.exec("PRIVMSG B :  \r\n", 4);
	expect_v.push_back("B");
	assert(result.cmd == "PRIVMSG");
	for(int i = 0; i < (int)expect_v.size();i++)
		assert(result.args[i] == expect_v[i]);
	assert(result.msg == "  ");
	expect_v.clear();

	// TODO 以下のケースはどういったデータを返すべきか？
	/*
	// error
	result = parser.exec("PRIVMSG B B B B B B B B B B B B B B B B:こんにちは\r\n", 4);
	// PRIVMSGを使用してクライアントに通達（コマンドの処理はしない）
	// 許容するか不明
	result = parser.exec("NICK         nusu\r\n", 4);
	result = parser.exec("PRIVMSG B :\r\n", 4);
	*/
	return (0);
}
