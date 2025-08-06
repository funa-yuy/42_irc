#include "Server.hpp"

Server::Server(int port)
{
	_server_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_server_fd < 0)
		exitError("socket: ");

	int	opt = 1;
	setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	std::memset(&_server_addr, 0, sizeof(_server_addr));
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_addr.s_addr = INADDR_ANY;
	_server_addr.sin_port = htons(port);

	if (bind(_server_fd, (struct sockaddr *)&_server_addr, sizeof(_server_addr)) < 0)
		exitError("bind: ");
	if (listen(_server_fd, MAX_CLIENTS))
		exitError("listen: ");

	struct pollfd pfd;
	pfd.fd = _server_fd;
	pfd.events = POLLIN;
	_poll_fds.push_back(pfd);
}

Server::~Server()
{
	close(_server_fd);
}

void	Server::run(void)
{
	std::cout << "Server is running..." << std::endl;
	while (true)
	{

		int	ret = poll(&_poll_fds[0], _poll_fds.size(), -1);
		if (ret < 0)
		{
			std::cerr << "poll: " << strerror(errno) << std::endl;
			break;
		}

		
		for (size_t i = 0; i < _poll_fds.size(); ++i)
		{
			if (_poll_fds[i].revents & POLLIN)
			{
				if (_poll_fds[i].fd == _server_fd)
					acceptNewClient();
				else
					handleClientInput(_poll_fds[i].fd);
			}
		}
	}
	return ;
}

void	Server::exitError(std::string const & error_msg)
{
	std::cerr << error_msg << strerror(errno) << std::endl;
	exit(1);
}

void	Server::acceptNewClient(void)
{
	int	client_fd = accept(_server_fd, NULL, NULL);
	if (client_fd < 0)
	{
		std::cerr << "accept: " << strerror(errno) << std::endl;
		return ;
	}

	struct pollfd pfd;
	pfd.fd = client_fd;
	pfd.events = POLLIN;
	_poll_fds.push_back(pfd);
	_client_buffers[client_fd] = "";
	std::cout << "New client connected: " << client_fd << std::endl;
	return ;
}

void	Server::handleClientInput(int fd)
{
	char	buf[BUF_SIZE];

	int		n = recv(fd, buf, BUF_SIZE - 1, 0);
	if (n <= 0)
	{
		removeClient(fd);
		return ;
	}

	buf[n] = '\0';
	_client_buffers[fd] += buf;

	size_t pos;
	while ((pos = _client_buffers[fd].find('\n')) != std::string::npos)
	{
		std::string msg = _client_buffers[fd].substr(0, pos + 1);
		_client_buffers[fd].erase(0, pos + 1);
		broadcast(fd, msg);
	}
	return ;
}

void	Server::broadcast(int sender_fd, std::string const & msg)
{
	for (size_t i = 1; i < _poll_fds.size(); ++i)
	{
		int fd = _poll_fds[i].fd;
		if (fd != sender_fd)
			send(fd, msg.c_str(), msg.size(), 0);
	}
	std::cout << "Broadcast from " << sender_fd << ": " << msg;
	return ;
}

void	Server::removeClient(int fd)
{
	std::cout << "Client disconnected: " << fd << std::endl;
	close(fd);

	for (size_t i = 0; i < _poll_fds.size(); ++i)
	{
		if (_poll_fds[i]. fd == fd)
		{
			_poll_fds.erase(_poll_fds.begin() + i);
			break ;
		}
	}
	_client_buffers.erase(fd);
}
