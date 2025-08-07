#ifndef PARSER_HPP
 #define PARSER_HPP

 #include <string>

class Parser
{
	public:
		Parser();
		// Parser(std::string line); // インスタンス化しないなら要らん
		Parser(const Parser& src);
		Parser& operator=(const Parser& src);
		~Parser();
		typedef struct s_parsed
		{
			int client_fd;
			std::string command;
			std::string args;
			std::string msg;
		}	t_parsed;
		// t_parsed exec(); // インスタンス化しないなら要らん
		t_parsed exec(std::string line, int client_fd);
	private:
		std::string _line;
};

#endif
