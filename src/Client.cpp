#include "Client.hpp"

Client::Client()
:	_nickname(""), _username(""),
	_passReceived(false), _nickReceived(false), _userReceived(false),
	_isRegistered(false), _isOperator(false)
{}

Client::~Client() {}

void	Client::initializeClient(int fd)
{
	_pfd.fd = fd;
	_pfd.events = POLLIN;
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
