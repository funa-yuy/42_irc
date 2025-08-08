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

int main()
{
	Parser parser;
	parser.exec("PRIVMSG B :こんにちは\r\n", 4);
	return (0);
}

//TODO 構造体の定義を別の場所にする
//todo インスタンスにするメリットがないのでサバと相談
/*
	メッセージ例）
	PRIVMSG B :こんにちは\r\n
*/
t_parsed Parser::exec(std::string line, int client_fd)
{
	std::string cmd;
	std::string args;
	std::string msg;
	std::stringstream stream(line);

	std::cout << "line:" << line;

	// vectorで空白スプリットを保管する
	std::vector<std::string> v;
	// std::string word;
	std::string str;
	while (getline(stream, str, ' '))
		v.push_back(str);

		for (const auto& w : v) {
		std::cout << "[" << w << "]" << std::endl;
	}

	// 先頭はコマンド（かプレフィックス）、最後は本文になる
	// 間の部分をすべて引数としたvectorで流す

	// stream << line;
	// std::cout << stream.str() << std::endl;

	// 動的に確保したほうがいい？freeする手間があるので微妙か？
	t_parsed parsed;

	// 一行ずつくる
	// 全長が512文字を超えないことを確認
	int len = line.size();
	// irssiでそもそも許容されるのか？
	// ncコマンドで接続してたら必要そう
	if (512 < len)
	{
		std::cout << "irc msg length over" << std::endl;
		return (parsed);
	}

	// コマンド＋引数＋メッセージ本体にわける

	// コマンドリスト（enum）と比較
	// 存在しないコマンドの場合はDEBUGlogに出力

}

t_parsed Parser::return_parse_err(std::string errmsg)
{
	// エラーよけ　そもそも返却する必要あるのか？
	t_parsed parsed;
	std::cerr << errmsg << std::endl;
	return (parsed);
}

void print_debug(std::string msg)
{
	std::cerr << "[DEBUG] " << msg << std::endl;
}

// lineってここで開放する必要ある？
t_parsed Parser::split_line(std::string line)
{
	t_parsed parsed;

	return (parsed);
}

std::string Parser::pick_cmd(std::string line)
{
	std::string cmd;
	// 空白で分ける

	return (cmd);
}
