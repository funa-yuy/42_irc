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

#include "Parser.hpp"

#include "Client.hpp"
#include "Database.hpp"

#include "Command/Command.hpp"
#include "Command/PassCommand.hpp"
#include "Command/NickCommand.hpp"
#include "Command/UserCommand.hpp"

#define MAX_CLIENTS 10
#define BUF_SIZE 512

class Server
{

public:

	Server(int port, std::string const & password);
	~Server();

	void	run(void);

private:

	int							_port;

	int							_server_fd;
	struct sockaddr_in			_server_addr;

	Database					_db;
	std::vector<struct pollfd>	_poll_fds;

	typedef Command*				(*_cmdFunc)();
	std::map<std::string, _cmdFunc>	_cmd_map;

	void		acceptNewClient(void);
	void		handleClientInput(int fd);
	void		disconnectClient(int fd);

	Command *	createCommandObj(std::string cmd_name);

	void		sendResponses(const t_response & res);
	bool		tryRegister(Client & client);
	void		sendWelcome(Client & client);
	
	std::string	displayNick(const Client & client) const;
	void		broadcast(int sender_fd, std::string const & msg);
	void		exitError(std::string const & error_msg);

};

#endif
