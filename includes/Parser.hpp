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
	std::string command;
	std::string args;
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

class Parser
{
	public:
		Parser();
		// Parser(std::string line); // インスタンス化しないなら要らん
		Parser(const Parser& src);
		Parser& operator=(const Parser& src);
		~Parser();
		// t_parsed exec(); // インスタンス化しないなら要らん
		t_parsed exec(std::string line, int client_fd);
		t_parsed return_parse_err(std::string errmsg);
		t_parsed split_line(std::string line);
		std::string pick_cmd(std::string line);
	private:
		std::string _line;
};

#endif
