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
Parser::t_parsed Parser::exec()
{
	// 一行ずつくる
	// コマンド＋引数＋メッセージ本体にわける
}