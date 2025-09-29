#include "Server.hpp"

Server::Server(int port, std::string const & password)
:
_port(port),
_db(password),
_last_ping(time(NULL)),
_ping_interval(PING_INTERVAL),
_timeout_ms(TIMEOUT_MS)
{
	_server_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_server_fd < 0)
		exitError("socket: ");

	fcntl(_server_fd, F_SETFL, O_NONBLOCK);
	signal(SIGPIPE, SIG_IGN);

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
		_cmd_map["CAP"] = &CapCommand::createCapCommand;
		_cmd_map["PASS"] = &PassCommand::createPassCommand;
		_cmd_map["NICK"] = &NickCommand::createNickCommand;
		_cmd_map["USER"] = &UserCommand::createUserCommand;
		_cmd_map["PING"] = &PingCommand::createPingCommand;
		_cmd_map["PONG"] = &PongCommand::createPongCommand;
		_cmd_map["PRIVMSG"] = &PrivmsgCommand::createPrivmsgCommand;
		_cmd_map["JOIN"] = &JoinCommand::createJoinCommand;
		_cmd_map["INVITE"] = &InviteCommand::createInviteCommand;
		_cmd_map["TOPIC"] = &TopicCommand::createTopicCommand;
		_cmd_map["MODE"] = &ModeCommand::createModeCommand;
		_cmd_map["KICK"] = &KickCommand::createKickCommand;
		_cmd_map["QUIT"] = &QuitCommand::createQuitCommand;
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

		int	ret = poll(&_poll_fds[0], _poll_fds.size(), _timeout_ms);
		if (ret < 0)
		{
			std::cerr << "poll: " << strerror(errno) << std::endl;
			break;
		}

		for (size_t i = 0; i < _poll_fds.size(); ++i)
		{
			if (_poll_fds[i].revents & (POLLHUP | POLLERR | POLLNVAL))
			{
				if (_poll_fds[i].fd != _server_fd)
				{
					disconnectClient(_poll_fds[i].fd);
					--i;
				}
				continue ;
			}
			if (_poll_fds[i].revents & POLLIN)
			{
				if (_poll_fds[i].fd == _server_fd)
					acceptNewClient();
				else
					handleClientInput(_poll_fds[i].fd);
			}
		}

		checkClientTimeout();

		time_t	now = time(NULL);
		if (now - _last_ping >= _ping_interval)
		{
			sendPing();
			_last_ping = now;
		}
	}

	return ;
}

void	Server::acceptNewClient(void)
{
	while (true)
	{
		int	client_fd = accept(_server_fd, NULL, NULL);
		if (client_fd < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break ;
			std::cerr << "accept: " << strerror(errno) << std::endl;
			return ;
		}
	
		fcntl(client_fd, F_SETFL, O_NONBLOCK);
	
		Client *	new_client = _db.addClient(client_fd);
		if (!new_client)
		{
			close(client_fd);
			continue ;
		}

		_poll_fds.push_back(new_client->getPfd());
	
		std::cout << "New client connected: " << new_client->getFd() << std::endl;
	}

	return ;
}

void	Server::handleClientInput(int fd)
{
	Client * client = _db.getClient(fd);
	if (!client)
		return ;

	bool	clientClosed = false;
	bool	fatalError = false;

	bool	dataReceived = readFromSocket(fd, client->getBuffer(), clientClosed, fatalError);

	if (clientClosed || fatalError)
	{
		disconnectClient(fd);
		return ;
	}
	if (!dataReceived)
		return ;

	std::cout << "\n \033[31m --- New data received --- \033[m" << std::endl;
	extractClientBufferLine(fd, client->getBuffer());
	std::cout << "\033[31m --- Receiving ends --- \033[m" << std::endl;

	return ;
}

bool	Server::readFromSocket(int fd, std::string & buffer, bool & clientClosed, bool & fatalError)
{
	clientClosed = false;
	fatalError = false;

	bool	dataReceived = false;
	char	buf[BUF_SIZE];

	while (true)
	{
		int n = recv(fd, buf, sizeof(buf), 0);
		if (n > 0)
		{
			buffer.append(buf, n);
			dataReceived = true;
			continue ;
		}
		if (n == 0)
		{
			clientClosed = true;
			return (dataReceived);
		}
		if (errno == EINTR)
			continue ;
		else if (errno == EAGAIN || errno == EWOULDBLOCK)
			return (dataReceived);
		fatalError = true;
		return (dataReceived);
	}
}

void	Server::extractClientBufferLine(int fd, std::string & buffer)
{
	while (true)
	{
		size_t pos = buffer.find('\n');
		if (pos == std::string::npos)
			break ;
		std::string msg = buffer.substr(0, pos + 1);
		buffer.erase(0, pos + 1);
		while (!msg.empty() && (msg.back() == '\n' || msg.back() == '\r'))
			msg.pop_back();
		if (executeCmdLine(fd, msg))
			return ;
		if (_db.getClient(fd) == NULL)
			return ;
	}
}

bool	Server::executeCmdLine(int fd, const std::string & msg)
{
	t_parsed parsed = Parser::exec(msg, fd);
	if (parsed.cmd.empty())
		return (false);
	Client * client = _db.getClient(fd);
	if (!client)
		return (true);

	PrintLog printlog;
	printlog.print_debug("INPUT LINE: " + msg);
	printlog.print_debug("COMMAND: " + parsed.cmd);
	for (int i = 0; i < (int)parsed.args.size(); ++i)
	{
		std::cerr << "[" << i << "]";
		printlog.print_debug("ARGUMENTS: " + parsed.args[i]);
	}

	if (!client->getIsRegistered())
	{
		if (parsed.cmd != "PASS" && parsed.cmd != "NICK" && parsed.cmd != "USER"
			&& parsed.cmd != "PONG" && parsed.cmd != "PING"
			&& parsed.cmd != "QUIT" && parsed.cmd != "CAP")
		{
			std::string not_registered = ":ft.irc 451 " + client->getNickname() + " :You have not registered\r\n";
			sendAllNonBlocking(parsed.client_fd, not_registered.c_str(), not_registered.size());
			return (false);
		}
	}

	Command * cmdObj = createCommandObj(parsed.cmd);
	if (cmdObj)
	{
		std::vector<t_response> responses = cmdObj->execute(parsed, _db);
		for (size_t i = 0; i < responses.size(); ++i)
		{
			const t_response & res = responses[i];
			if (res.should_disconnect)
			{
				sendResponses(res);
				delete cmdObj;
				disconnectClient(parsed.client_fd);
				return (true);
			}
			sendResponses(res);
		}
		client = _db.getClient(fd);
		if (client)
			tryRegister(*client);
		delete cmdObj;
	}
	else
	{
		std::string unknown = ":ft.irc 421 " + displayNick(*client) + " " + parsed.cmd + " :Unknown command\r\n";
		sendAllNonBlocking(parsed.client_fd, unknown.c_str(), unknown.size());
	}
	return (false);
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

	std::vector<std::string> names = _db.getAllChannelNames();
	for (size_t i = 0; i < names.size(); ++i)
	{
		Channel * ch = _db.getChannel(names[i]);
		if (!ch)
			continue ;
		if (ch->isInvited(fd))
			ch->removeInvite(fd);
		if (ch->isMember(fd))
		{
			ch->removeClientFd(fd);
			if (ch->getClientFds().empty())
				_db.removeChannel(names[i]);
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
			sendAllNonBlocking(fd, res.reply.c_str(), res.reply.size());
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

bool	Server::sendAllNonBlocking(int fd, const char * data, size_t len)
{
	size_t sent = 0;

	while (sent < len)
	{
		ssize_t n = send(fd, data + sent, len - sent, 0);
		if (n > 0)
		{
			sent += static_cast<size_t>(n);
			continue ;
		}
		if (n < 0)
		{
			if (errno == EINTR)
				continue ;
			else if (errno == EAGAIN || errno == EWOULDBLOCK)
				return (false);
			return (false);
		}
	}
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

	sendAllNonBlocking(client.getFd(), welcome.c_str(), welcome.size());

	return ;
}

void	Server::sendPing(void)
{
	std::string	token = "Hey! Are you there?";
	for
	(
		std::map<int, Client>::iterator it = _db.getAllClient().begin();
		it != _db.getAllClient().end();
		++it
	)
	{
		int	fd = it->first;
		std::string	ping = "PING :" + token + "\r\n";
		sendAllNonBlocking(fd, ping.c_str(), ping.size());
		it->second.setLastPingToken(token);
	}
	return ;
}

void	Server::checkClientTimeout(void)
{
	time_t	now = time(NULL);
	for
	(
		std::map<int, Client>::iterator	it = _db.getAllClient().begin();
		it != _db.getAllClient().end();

	)
	{
		if (it->second.getLastPingTime() != 0
			&& (now - it->second.getLastPingTime()) > _ping_interval * 2)
		{
			int	fd = it->second.getFd();
			++it;
			disconnectClient(fd);
			continue ;
		}
		++it;
	}
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
			sendAllNonBlocking(fd, msg.c_str(), msg.size());
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
		if (_poll_fds[i].revents & (POLLHUP | POLLERR | POLLNVAL))
		{
			if (_poll_fds[i].fd != _server_fd)
			{
				disconnectClient(_poll_fds[i].fd);
				--i;
			}
			continue ;
		}
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
