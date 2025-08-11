#include "Client.hpp"

Client::Client()
:	_nickname(""), _username(""),
	_passReceived(false), _nickReceived(false), _userReceived(false),
	_isRegistered(false), _isOperator(false)
{}

Client::~Client() {}

void	Client::setClientFd(int fd)
{
	_pfd.fd = fd;
	_pfd.events = POLLIN;
}

int	Client::getFd(void) const
{
	return (_pfd.fd);
}

struct pollfd	Client::getPfd(void) const
{
	return (_pfd);
}
