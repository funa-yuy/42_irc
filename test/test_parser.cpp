# include "../includes/Server.hpp"
# include "../includes/Parser.hpp"
#include <cassert>
#include <sstream>

int main()
{
	Parser parser;
	t_parsed result;
	std::vector<std::string> expect_v;

	// test
	result = parser.exec("PRIVMSG B :こんにちは\r\n", 4);
	expect_v.push_back("B");
	expect_v.push_back("こんにちは");
	assert(result.cmd == "PRIVMSG");
	for(int i = 0; i < (int)expect_v.size();i++)
		assert(result.args[i] == expect_v[i]);
	expect_v.clear();

	// no msg
	result = parser.exec("PASS hunter2\r\n", 4);
	expect_v.push_back("hunter2");
	assert(result.cmd == "PASS");
	for(int i = 0; i < (int)expect_v.size();i++)
		assert(result.args[i] == expect_v[i]);
	expect_v.clear();

	// multi
	result = parser.exec("USER nusu 0 * :Nusu Realname\r\n", 4);
	expect_v.push_back("nusu");
	expect_v.push_back("0");
	expect_v.push_back("*");
	expect_v.push_back("Nusu Realname");
	assert(result.cmd == "USER");
	for(int i = 0; i < (int)expect_v.size();i++)
		assert(result.args[i] == expect_v[i]);
	expect_v.clear();

	result = parser.exec("MODE #chatroom +o user123\r\n", 4);
	expect_v.push_back("#chatroom");
	expect_v.push_back("+o");
	expect_v.push_back("user123");
	assert(result.cmd == "MODE");
	for(int i = 0; i < (int)expect_v.size();i++)
		assert(result.args[i] == expect_v[i]);
	expect_v.clear();

	// space only msg
	result = parser.exec("PRIVMSG B :  \r\n", 4);
	expect_v.push_back("B");
	expect_v.push_back("  ");
	assert(result.cmd == "PRIVMSG");
	for(int i = 0; i < (int)expect_v.size();i++)
		assert(result.args[i] == expect_v[i]);
	expect_v.clear();

	// multi ':' in msg
	result = parser.exec("PRIVMSG B ::aaaa\r\n", 4);
	expect_v.push_back("B");
	expect_v.push_back(":aaaa");
	assert(result.cmd == "PRIVMSG");
	for(int i = 0; i < (int)expect_v.size();i++)
		assert(result.args[i] == expect_v[i]);
	expect_v.clear();

	// multi ':' in msg
	result = parser.exec("PRIVMSG B :42tokyo :hello\r\n", 4);
	expect_v.push_back("B");
	expect_v.push_back("42tokyo :hello");
	assert(result.cmd == "PRIVMSG");
	for(int i = 0; i < (int)expect_v.size();i++)
		assert(result.args[i] == expect_v[i]);
	expect_v.clear();

	result = parser.exec("PRIVMSG nick:42tokyo hello world\r\n", 4);
	expect_v.push_back("nick:42tokyo");
	expect_v.push_back("hello world");
	assert(result.cmd == "PRIVMSG");
	for(int i = 0; i < (int)expect_v.size();i++)
		assert(result.args[i] == expect_v[i]);
	expect_v.clear();

	// multi space in message
	result = parser.exec("NICK         nusu\r\n", 4);
	expect_v.push_back("nusu");
	assert(result.cmd == "NICK");
	for(int i = 0; i < (int)expect_v.size();i++)
		assert(result.args[i] == expect_v[i]);
	expect_v.clear();

	// multi space in message
	result = parser.exec("NICK         nusu    aaaa\r\n", 4);
	expect_v.push_back("nusu");
	expect_v.push_back("aaaa");
	assert(result.cmd == "NICK");
	for(int i = 0; i < (int)expect_v.size();i++)
		assert(result.args[i] == expect_v[i]);
	expect_v.clear();

	// no msg
	result = parser.exec("PRIVMSG B :\r\n", 4);
	expect_v.push_back("B");
	assert(result.cmd == "PRIVMSG");
	for(int i = 0; i < (int)expect_v.size();i++)
		assert(result.args[i] == expect_v[i]);
	expect_v.clear();

	//引数が15個以上
	{
		std::stringstream ss;
		ss << "CMD";
		for (int i = 1; i <= 20; ++i) {
			ss << " p" << i;
		}
		ss << "\r\n";
		result = parser.exec(ss.str(), 4);
		assert(result.cmd == "CMD");
		assert((int)result.args.size() == 15);
		for (int i = 0; i < 15; ++i) {
			std::stringstream es;
			es << "p" << (i + 1);
			assert(result.args[i] == es.str());
		}
	}

	// 512文字以上
	{
		const std::string prefix = "CMD ";
		std::string big(600, 'x');
		std::string line = prefix + big + " :こんにちわ\r\n";
		result = parser.exec(line, 4);
		assert(result.cmd == "CMD");
		assert((int)result.args.size() == 1);
		size_t expected = MAX_LINE_NOCRLF - prefix.size();
		assert(result.args[0].size() == expected);
		assert(result.args[0] == big.substr(0, expected));
	}

	//  512文字以上
	{
		const std::string prefix = "CMD B :";
		std::string big(600, 'x');
		std::string line = prefix + big + "\r\n";
		result = parser.exec(line, 4);
		assert(result.cmd == "CMD");
		assert((int)result.args.size() == 2);
		assert(result.args[0] == "B");
		size_t expected = MAX_LINE_NOCRLF - prefix.size();
		assert(result.args[1].size() == expected);
		assert(result.args[1] == big.substr(0, expected));
	}

	//PRIVMSG
	{
		result = parser.exec("PRIVMSG 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15:こんにちは\r\n", 4);
		assert(result.cmd == "PRIVMSG");
		assert((int)result.args.size() == 2);
		assert(result.args[0] == "0");
		assert(result.args[1] == "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15:こんにちは");
	}

	std::cout << "parse tests: OK" << std::endl;
	return (0);
}
