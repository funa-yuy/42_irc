#ifndef IRC_HPP
# define IRC_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>
#include <vector>

// ------------------------------------------------
// struct
// ------------------------------------------------

typedef struct	s_response
{
	std::string	reply;
	int			*target_fds;
	int			send_flag;
}				t_response;

typedef struct	s_parserd
{
	// char*	msg; //一時的に、recv()で受け取ったものそのままを格納。
	std::string					cmd;
	int							sender_fd;
	std::vector<std::string>	option;
}				t_parserd;


// ------------------------------------------------
// function
// ------------------------------------------------


#endif
