#ifndef COMMAND_HPP
# define COMMAND_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>

// ------------------------------------------------
// struct
// ------------------------------------------------

typedef struct	s_response
{
	std::string	msg;
	int			*fds;
	int			send_flag;
}				t_response;

//todo: 一時的に作成。本来はぬすさんが定義したものを使用する
typedef struct	s_parserd
{
	char*	input;
	int		client_fd;
}				t_parserd;


// ------------------------------------------------
// class
// ------------------------------------------------


class Command {
	public:
		Command();
		virtual ~Command() = 0;//純粋仮想関数

		virtual const t_response	execute(char* input) const = 0; //純粋仮想関数

};

// ------------------------------------------------
// function
// ------------------------------------------------


#endif
