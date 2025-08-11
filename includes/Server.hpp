#ifndef SERVER_HPP
# define SERVER_HPP

#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/poll.h>

#include "Database.hpp"

#include "Command.hpp"
#include "PassCommand.hpp"
#include "NickCommand.hpp"
#include "UserCommand.hpp"

#define MAX_CLIENTS 10
#define BUF_SIZE 512

class Server
{

public:

	Server(int port, std::string const & password);
	~Server();

	void	run(void);

private:

	std::string					_password;

	int							_server_fd;
	struct sockaddr_in			_server_addr;

	std::vector<struct pollfd>	_poll_fds;
	std::map<int, std::string>	_client_buffers;

	typedef Command*				(*_cmdFunc)();
	std::map<std::string, _cmdFunc>	_cmd_map;

	void		acceptNewClient(void);
	void		handleClientInput(int fd);
	void		broadcast(int sender_fd, std::string const & msg);
	void		removeClient(int fd);
	void		exitError(std::string const & error_msg);
	Command *	createCommandObj(std::string cmd_name);

};

#endif
