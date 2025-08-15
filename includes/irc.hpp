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
	bool				is_success;
	bool				should_send;
	std::string			reply;
	std::vector<int>	target_fds;
	int					send_flag;
}				t_response;

typedef struct	s_parsed
{
	int client_fd;
	std::string cmd;
	std::vector<std::string> args;
	std::string msg;
}				t_parsed;

// ------------------------------------------------
// function
// ------------------------------------------------


#endif
