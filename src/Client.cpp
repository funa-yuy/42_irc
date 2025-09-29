#include "Client.hpp"

Client::Client()
:	_fd(0), _buffer(""),
	_nickname(""), _username(""), _realname(""),
	_passReceived(false), _nickReceived(false), _userReceived(false),
	_isRegistered(false), _isOperator(false),
	_lastPingTime(time(NULL)), _lastPingToken("")
{
	_pfd.fd = 0;
	_pfd.events = 0;
	_pfd.revents = 0;
}

Client::~Client() {}

void	Client::initializeClient(int fd)
{
	_pfd.fd = fd;
	_pfd.events = POLLIN;
	_pfd.revents = 0;
	_fd = fd;
	_buffer = "";

	return ;
}

int	Client::getFd(void) const
{
	return (_fd);
}

struct pollfd	Client::getPfd(void) const
{
	return (_pfd);
}

std::string &	Client::getBuffer(void)
{
	return (_buffer);
}

std::string const &	Client::getBuffer(void) const
{
	return (_buffer);
}

std::string	Client::getNickname() const
{
	return (_nickname);
}

void	Client::setNickname(const std::string & nickname)
{
	_nickname = nickname;
}

std::string	Client::getUsername(void) const
{
	return (_username);
}

std::string	Client::getRealname(void) const
{
	return (_realname);
}

bool	Client::getUserReceived(void) const
{
	return (_userReceived);
}

bool	Client::getIsOperator(void) const
{
	return (_isOperator);
}

bool	Client::getIsRegistered() const {
	return (_isRegistered);
}


time_t	Client::getLastPingTime(void) const
{
	return (_lastPingTime);
}

std::string	Client::getLastPingToken(void) const
{
	return (_lastPingToken);
}

void	Client::setFd(int fd)
{
	_fd = fd;
	return ;
}

void	Client::setUsername(const std::string & username)
{
	_username = username;
	return ;
}

void	Client::setRealname(const std::string & realname)
{
	_realname = realname;
	return ;
}

void	Client::setUserReceived(bool val)
{
	_userReceived = val;
	return ;
}

void	Client::setIsRegistered(bool val)
{
	_isRegistered = val;
	return ;
}

bool	Client::getPassReceived(void) const
{
	return (_passReceived);
}

bool	Client::getNickReceived(void) const
{
	return (_nickReceived);
}

void	Client::setPassReceived(bool val)
{
	_passReceived = val;
	return ;
}

void	Client::setNickReceived(bool val)
{
	_nickReceived = val;
	return ;
}

void	Client::setLastPingTime(time_t time)
{
	_lastPingTime = time;
	return ;
}

void	Client::setLastPingToken(std::string token)
{
	_lastPingToken = token;
	return ;
}
