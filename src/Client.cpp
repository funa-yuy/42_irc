#include "Client.hpp"

Client::Client() {}

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
