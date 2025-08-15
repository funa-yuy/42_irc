#include "Server.hpp"

Server::Server(int port, std::string const & password)
: _port(port), _db(password)
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

	struct pollfd server_pfd;
	server_pfd.fd = _server_fd;
	server_pfd.events = POLLIN;
	server_pfd.revents = 0;
	_poll_fds.push_back(server_pfd);

	// if (_cmd_map.empty())
	// {
	// 	_cmd_map["PASS"] = &PassCommand::createCommand;
	// 	_cmd_map["NICK"] = &NickCommand::createCommand;
	// 	_cmd_map["USER"] = &UserCommand::createCommand;
	// }
}

Server::~Server()
{
	close(_server_fd);
}

void	Server::run(void)
{
	std::cout << "Server is running..." << std::endl;
	std::cout << "Port number: " << _port << std::endl;
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

	Client *	new_client = _db.addClient(client_fd);
	if (!new_client)
		return ;

	_poll_fds.push_back(new_client->getPfd());

	std::cout << "New client connected: " << new_client->getFd() << std::endl;
	return ;
}

void	Server::handleClientInput(int fd)
{
	char	buf[BUF_SIZE];
	int		n;

	n = recv(fd, buf, BUF_SIZE - 1, 0);
	if (n <= 0)
	{
		disconnectClient(fd);
		return ;
	}

	std::cout << "\n \033[31m --- New data received --- \033[m" << std::endl;

	buf[n] = '\0';
	Client *	client = _db.getClient(fd);
	if (!client)
		return ;

	std::string &	clientBuffer = client->getBuffer();
	clientBuffer += buf;

	size_t pos;
	while ((pos = clientBuffer.find('\n')) != std::string::npos)
	{
		std::string msg = clientBuffer.substr(0, pos + 1); // msgに_client_buffersの先頭から'\n'までを分ける
		clientBuffer.erase(0, pos + 1); // msgに分けた部分を_client_buffersから削除、次のwhile反復で「次の'\n'」を探せる
		while (!msg.empty() && (msg[msg.size() - 1] == '\n' || msg[msg.size() - 1] == '\r')) // msg内の'\n'と'\r'を削除
			msg.erase(msg.size() - 1);

		t_parsed	parsed;
		parsed = Parser::exec(msg, fd);

		std::cout << "\n[RECV fd=" << parsed.client_fd << "] " << std::endl;
		std::cout << msg << std::endl;

		std::cout << "COMMAND: " << std::endl;
		std::cout << parsed.cmd << std::endl;

		std::cout << "ARGUMENTS: " << std::endl;
		for (size_t i = 0; i < parsed.args.size(); i++)
			std::cout << i << ": " << parsed.args[i] << std::endl;

		// Command * cmdObj = createCommandObj(cmd);
		// t_parsed	parsed;
		// if (cmdObj)
		// {
		// 	cmdObj->execute(parsed);
		// 	delete cmdObj;
		// }
		// else
		// {
		// 	// 知らないコマンド
		// }
	}
	std::cout << "\n \033[31m --- Receiving ends --- \033[m" << std::endl;
	return ;
}

void	Server::broadcast(int client_fd, std::string const & msg)
{
	for (size_t i = 1; i < _poll_fds.size(); ++i)
	{
		int fd = _poll_fds[i].fd;
		if (fd != client_fd)
			send(fd, msg.c_str(), msg.size(), 0);
	}
	std::cout << "Broadcast from " << client_fd << ": " << msg;
	return ;
}

void	Server::disconnectClient(int fd)
{
	std::cout << "Client disconnected: " << fd << std::endl;
	close(fd);

	for (size_t i = 0; i < _poll_fds.size(); ++i)
	{
		if (_poll_fds[i].fd == fd)
		{
			_poll_fds.erase(_poll_fds.begin() + i);
			break ;
		}
	}

	_db.removeClient(fd);

	return ;
}

Command *	Server::createCommandObj(std::string cmd_name)
{
	std::map<std::string, _cmdFunc>::iterator it = _cmd_map.find(cmd_name);
	if (it != _cmd_map.end())
		return (it->second());
	return (NULL);
}
