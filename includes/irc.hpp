#ifndef IRC_HPP
# define IRC_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>
#include <vector>

// ------------------------------------------------
// constants
// ------------------------------------------------

#define USERLEN 10
#define DEFAULT_REALNAME "*"

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
	bool				is_success;
	bool				should_send;
	bool				should_disconnect;
	std::string			reply;
	std::vector<int>	target_fds;
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
