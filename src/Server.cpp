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

	if (_cmd_map.empty())
	{
		_cmd_map["PASS"] = &PassCommand::createPassCommand;
		_cmd_map["NICK"] = &NickCommand::createNickCommand;
		_cmd_map["USER"] = &UserCommand::createUserCommand;
		_cmd_map["PING"] = &PongCommand::createPongCommand;
		_cmd_map["PRIVMSG"] = &PrivmsgCommand::createPrivmsgCommand;
	}
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
		std::string msg = clientBuffer.substr(0, pos + 1);
		clientBuffer.erase(0, pos + 1);
		while (!msg.empty() && (msg[msg.size() - 1] == '\n' || msg[msg.size() - 1] == '\r'))
			msg.erase(msg.size() - 1);

		t_parsed	parsed = Parser::exec(msg, fd);
		if (parsed.cmd.empty())
			continue ;

		PrintLog printlog;
		printlog.print_debug("INPUT LINE: " + msg);
		printlog.print_debug("COMMAND: " + parsed.cmd);
		for (int i = 0; i < (int)parsed.args.size(); ++i)
		{
			std::cerr << "[" << i << "] ";
			printlog.print_debug("ARGUMENT: " + parsed.args[i]);
		}

		if (!client->getIsRegistered())
		{
			if (parsed.cmd != "PASS" && parsed.cmd != "NICK" && parsed.cmd != "USER"
				&& parsed.cmd != "PING" && parsed.cmd != "QUIT" && parsed.cmd != "CAP")
			{
				std::string not_registered = ":ft.irc 451 " + displayNick(*client) + " :You have not registered\r\n";
				send(parsed.client_fd, not_registered.c_str(), not_registered.size(), 0);
				continue ;
			}
		}

		Command * cmdObj = createCommandObj(parsed.cmd);
		if (cmdObj)
		{
			std::vector<t_response> response_list = cmdObj->execute(parsed, _db);
			bool should_break_outer = false;
			for (size_t i = 0; i < response_list.size(); ++i)
			{
				const t_response & res = response_list[i];
				if (res.should_disconnect)
				{
					sendResponses(res);
					delete cmdObj;
					disconnectClient(parsed.client_fd);
					should_break_outer = true;
					break;
				}
				sendResponses(res);
			}
			if (should_break_outer)
				break ;
			tryRegister(*client);
			delete cmdObj;
		}
		else
		{
			// 知らないコマンド
			std::string unknown = ":ft.irc 421 " + displayNick(*client) + " " + parsed.cmd + " :Unknown command\r\n";
			send(parsed.client_fd, unknown.c_str(), unknown.size(), 0);
		}
	}
	std::cout << "\033[31m --- Receiving ends --- \033[m" << std::endl;

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

void	Server::sendResponses(const t_response & res)
{
	if (!res.should_send)
		return ;
	PrintLog printlog;
	printlog.print_debug("SEND REPLY: " + res.reply);
	for (size_t i = 0; i < res.target_fds.size(); ++i)
	{
		int fd = res.target_fds[i];
		if (fd >= 0 && !res.reply.empty())
			send(fd, res.reply.c_str(), res.reply.size(), 0);
	}

	return ;
}

bool	Server::tryRegister(Client & client)
{
	if (client.getIsRegistered())
		return (false);
	if (!client.getPassReceived() || !client.getNickReceived() || !client.getUserReceived())
		return (false);
	client.setIsRegistered(true);

	std::cout	<< "[REGISTERED] fd = " << client.getFd()
	<< " nick = " << displayNick(client)
	<< " user = " << client.getUsername()
	<< " realname = " << client.getRealname()
	<< std::endl;

	sendWelcome(client);

	return (true);
}

void	Server::sendWelcome(Client & client)
{
	std::string nickname = displayNick(client);
	std::string	welcome;

	welcome = ":ft.irc 001 " + nickname + " :Welcome to the ft_irc Network "
	+ nickname + "!" + client.getUsername() + "@ft.irc\r\n";
	welcome += ":ft.irc 002 " + nickname + " :Your host is ft.irc, running version 0.1\r\n";
	welcome += ":ft.irc 003 " + nickname + " :This server was created 2025-08-26\r\n";
	welcome += ":ft.irc 004 " + nickname + " ft.irc 0.1 o o\r\n";

	send(client.getFd(), welcome.c_str(), welcome.size(), 0);

	return ;
}

std::string	Server::displayNick(const Client & client) const
{
	std::string nick = client.getNickname();
	if (nick.empty())
		return "*";

	return (nick);
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

void	Server::exitError(std::string const & error_msg)
{
	std::cerr << error_msg << strerror(errno) << std::endl;
	exit(1);
}

bool	Server::step(int timeout_ms)
{
    int ret = poll(&_poll_fds[0], _poll_fds.size(), timeout_ms);
    if (ret < 0)
    {
        std::cerr << "poll: " << strerror(errno) << std::endl;
        return false;
    }
    if (ret == 0) // timeout
        return true;

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
    return true;
}
