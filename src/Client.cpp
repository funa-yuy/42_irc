#include "Client.hpp"

Client::Client()
:	_fd(0), _buffer(""),
	_nickname(""), _username(""),
	_passReceived(false), _nickReceived(false), _userReceived(false),
	_isRegistered(false), _isOperator(false)
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

void	Client::setNickname(std::string nickname)
{
	_nickname = nickname;
}