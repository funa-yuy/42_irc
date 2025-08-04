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
