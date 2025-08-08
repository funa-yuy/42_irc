#ifndef PARSER_HPP
 #define PARSER_HPP

 #include <string>
 #include <iostream>
 #include <sstream>
 #include <vector>

 // コマンド呼び出し側と共有する
 typedef struct s_parsed
{
	int client_fd;
	std::string cmd;
	std::vector<std::string> args;
	std::string msg;
}	t_parsed;

typedef enum
{
	PASS,
	NICK,
	USER,
	JOIN,
	PRIVMSG,
	OPER,
	KICK,
	INVITE,
	TOPIC,
	MODE
}	cmds_l;

void print_debug(std::string msg);

class Parser
{
	public:
		Parser();
		Parser(const Parser& src);
		Parser& operator=(const Parser& src);
		~Parser();
		t_parsed exec(std::string line, int client_fd);
	private:
		std::string _line;
};

#endif
