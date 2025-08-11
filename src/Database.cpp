#include "Database.hpp"

Database::Database() {}

Database::~Database() {}

Client *	Database::addClient(int fd)
{
	Client new_client;
	new_client.setClientFd(fd);
	_clients[fd] = new_client;

	return (&_clients[fd]);
}

void	Database::removeClient(int fd)
{
	_clients.erase(fd);
	return ;
}
