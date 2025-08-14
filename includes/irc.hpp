#ifndef IRC_HPP
# define IRC_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>
#include <vector>

// ------------------------------------------------
// enum
// ------------------------------------------------

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
	std::string					cmd;
	int							sender_fd;
	std::vector<std::string>	option;
}				t_parserd;

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
