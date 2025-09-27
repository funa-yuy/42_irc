#include "Database.hpp"

Database::Database(std::string password)
: _password(password)
{}

Database::~Database() {}

Client *	Database::addClient(int fd)
{
	Client new_client;
	new_client.initializeClient(fd);
	_clients[fd] = new_client;

	return (&_clients[fd]);
}

void	Database::removeClient(int fd)
{
	_clients.erase(fd);
	return ;
}

Client *	Database::getClient(int fd)
{
	std::map<int, Client>::iterator it = _clients.find(fd);
	if (it == _clients.end())
		return (NULL);
	return (&it->second);
}

Client const *	Database::getClient(int fd) const
{
	std::map<int, Client>::const_iterator it = _clients.find(fd);
	if (it == _clients.end())
		return (NULL);
	return (&it->second);
}

Client *	Database::getClient(const std::string & nickname)
{
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second.getNickname() == nickname)
			return (&it->second);
	}
	return (NULL);
}

Client const *	Database::getClient(const std::string & nickname) const
{
	for (std::map<int, Client>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second.getNickname() == nickname)
			return (&it->second);
	}
	return (NULL);
}

const std::string&	Database::getPassword() const {
	return (_password);
}

std::map<int, Client>&		Database::getAllClient()
{
	return (_clients);
}

void Database::addChannel(Channel& channel)
{
	_channels[channel.getName()] = channel;
}

Channel *		Database::getChannel(const std::string & name)
{
	std::string normalized = toLowerCase(name);
	std::map<std::string, Channel>::iterator it = _channels.find(normalized);
	if (it == _channels.end())
		return (NULL);
	return (&it->second);
}

const Channel *		Database::getChannel(const std::string& name) const
{
	std::string normalized = toLowerCase(name);
	std::map<std::string, Channel>::const_iterator it = _channels.find(normalized);
	if (it == _channels.end())
		return (NULL);
	return (&it->second);
}

void Database::removeChannel(std::string& name)
{
	std::string normalized = toLowerCase(name);
	std::map<std::string, Channel>::iterator it = _channels.find(normalized);
	if (it != _channels.end())
		_channels.erase(it);
}

std::vector<std::string> Database::getAllChannelNames() const
{
    std::vector<std::string> names;
    for (std::map<std::string, Channel>::const_iterator it = _channels.begin(); it != _channels.end(); ++it)
        names.push_back(it->first);
    return (names);
}
