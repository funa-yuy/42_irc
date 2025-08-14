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
	bool		is_success;
	bool		should_send;
	std::string	reply;
	int			*target_fds;
	int			send_flag;
}				t_response;

typedef struct	s_parserd
{
	std::string					cmd;
	int							sender_fd;
	std::vector<std::string>	option;
}				t_parserd;


// ------------------------------------------------
// function
// ------------------------------------------------


#endif
