# include "../includes/Server.hpp"
# include "../includes/Parser.hpp"

Parser::Parser() {}

// Parser::Parser(std::string line) :_line(line) {}

Parser::Parser(const Parser& src)
{
	_line = src._line;
}

Parser& Parser::operator=(const Parser& src)
{
	_line = src._line;
	return (*this);
}

Parser::~Parser() {}

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
	// error
	result = parser.exec("PRIVMSG B B B B B B B B B B B B B B B B:こんにちは\r\n", 4);
	// 許容する？
	result = parser.exec("NICK         nusu\r\n", 4);
	result = parser.exec("PRIVMSG B :\r\n", 4);
	return (0);
}

// TODO vectorのメモリリークについて調べる
t_parsed Parser::exec(std::string line, int client_fd)
{
	t_parsed parsed;
	std::stringstream stream(line);
	std::vector<std::string> v;

	int pos = line.rfind(":", line.size() - 1);
	if (pos < 0)
		parsed.msg = "";
	else
	{
		parsed.msg = line.substr(pos);
		parsed.msg.erase(0, 1);
		parsed.msg.erase(parsed.msg.size() - 2, 2);
		line.erase(pos);
	}

	std::string temp;
	while (getline(stream, temp, ' '))
	{
		if (temp.size() >= 2 && temp.compare(temp.size() - 2, 2, "\r\n") == 0)
			temp.erase(temp.size() - 2);
		v.push_back(temp);
	}
	parsed.cmd = v[0];
	v.erase(v.begin());
	if (15 < v.size())
		print_debug("too many args");
	parsed.args = v;
	parsed.client_fd = client_fd;
	return (parsed);
}

void print_debug(std::string msg)
{
	std::cerr << "[DEBUG] " << msg << std::endl;
}
