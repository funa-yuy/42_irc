# include "Server.hpp"
# include "Parser.hpp"

Parser::Parser() {}

Parser::Parser(std::string line) :_line(line) {}

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

//TODO 構造体の定義を別の場所にする
//todo これどうやって使うつもり？インスタンスにするメリットなくね？
Parser::t_parsed Parser::exec(std::string line, int client_fd)
{
	std::string cmd;
	std::string args;
	std::string msg;

	// 動的に確保したほうがいい？freeする手間があるので微妙か？
	t_parsed parsed;

	// 一行ずつくる
	// 全長が512文字を超えないことを確認
	int len = line.size();
	// irssiでそもそも許容されるのか？
	// ncコマンドで接続してたら必要そう
	if (512 < len)
	{
		std::cout << "" << std::endl;
		return (parsed);
	}
	// コマンド＋引数＋メッセージ本体にわける
	// コマンドリスト（enum）と比較
	// 存在しないコマンドの場合はDEBUGlogに出力

}

Parser::t_parsed Parser::return_parse_err(std::string errmsg)
{
	std::cout << errmsg << std::endl;
	return (NULL);
}

void print_debug(std::string msg)
{
	std::cout << "[DEBUG]" << msg << std::endl;
}