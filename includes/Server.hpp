#ifndef SERVER_HPP
# define SERVER_HPP

#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/poll.h>

#define PORT 6667
#define MAX_CLIENTS 10
#define BUF_SIZE 512

class Server
{

public:
	
	Server(int port);
	~Server();

	void	run(void);

private:

	int							_server_fd;
	struct sockaddr_in			_server_addr;

	std::vector<struct pollfd>	_poll_fds;
	std::map<int, std::string>	_client_buffers;

	void	acceptNewClient(void);
	void	handleClientInput(int fd);
	void	broadcast(int sender_fd, std::string const & msg);
	void	removeClient(int fd);
	void	exitError(std::string const & error_msg);

};

#endif
