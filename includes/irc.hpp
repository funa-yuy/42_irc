#ifndef IRC_HPP
# define IRC_HPP

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

	// int		client_fd;
}				t_parserd;


// ------------------------------------------------
// function
// ------------------------------------------------


#endif
