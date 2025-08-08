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

	// test
	result = parser.exec("PRIVMSG B :こんにちは\r\n", 4);
	assert(result.cmd == "PRIVMSG");
	assert();
	assert(result.msg == "こんにちは");

	result = parser.exec("PASS hunter2\r\n", 4);
	result = parser.exec("NICK nusu\r\n", 4);
	result = parser.exec("USER nusu 0 * :Nusu Realname\r\n", 4);
	result = parser.exec("JOIN #chatroom\r\n", 4);
	result = parser.exec("PRIVMSG #chatroom :Hello everyone!\r\n", 4);
	result = parser.exec("OPER admin secretpass\r\n", 4);
	result = parser.exec("KICK #chatroom user123 :Spamming\r\n", 4);
	result = parser.exec("INVITE user123 #chatroom\r\n", 4);
	result = parser.exec("TOPIC #chatroom :New topic here!\r\n", 4);
	result = parser.exec("MODE #chatroom +o user123\r\n", 4);

	// error
	result = parser.exec("PRIVMSG B B B B B B B B B B B B B B B B:こんにちは\r\n", 4);

	// 許容する？リファレンス確認
	result = parser.exec("NICK         nusu\r\n", 4);
	result = parser.exec("PRIVMSG B :\r\n", 4);
	return (0);
}

/*
	メッセージ例）
	PRIVMSG B :こんにちは\r\n
*/
// TODO vectorのメモリリークについて調べる
t_parsed Parser::exec(std::string line, int client_fd)
{
	t_parsed parsed;
	std::stringstream stream(line);
	std::vector<std::string> v;
	std::string str;

	// 後ろから:始まりの部分を本文として抜き出す
	// 本文がないコマンドの場合はどうなる？
	int pos = line.rfind(":", line.size() - 1);
	std::cout << "pos " << pos << std::endl;
	if (pos < 0)
		parsed.msg = "";
	else
	{
		parsed.msg = line.substr(pos);
		// trim /r/n && trim :
		line.erase(pos);
	}

	// vectorで空白スプリットを保管する
	while (getline(stream, str, ' '))
		v.push_back(str);

	// 確認用
	for (int i = 0; i < v.size();i++) {
		std::cout << "[" << v[i] << "]" << std::endl;
	}

	// 先頭はコマンド（かプレフィックス）、最後は本文になる
	// 間の部分をすべて引数としたvectorで流す
	parsed.cmd = v[0];
	// vectorの先頭と末尾を削除
	v.erase(v.end());
	v.erase(v.begin());

	std::cout << "cmd: " << parsed.cmd << std::endl;
	std::cout << "msg: " << parsed.msg << std::endl;

	// コマンドリスト（enum）と比較　// サーバーで担保する

	// 引数の数は15まで許容する
	if (15 < v.size())
	{
		print_debug("too many args");
	}
	parsed.args = v;

	return (parsed);
}

void print_debug(std::string msg)
{
	std::cerr << "[DEBUG] " << msg << std::endl;
}
