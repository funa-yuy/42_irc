#include "Channel.hpp"

std::string Channel::getName() const
{
	return (_name);
}

std::map<int, Client> Channel::getClients() const
{
	return (_clients);
}

std::string Channel::getTopic() const
{
	return (_topic);
}

Client *Channel::getChannelOperator() const
{
	return (_channelOperator);
}

void Channel::setName(std::string name)
{
	_name = name;
}

void Channel::addClient(Client& client)
{
	_clients[client.getFd()] = client;
}

void Channel::setTopic(std::string topic)
{
	_topic = topic;
}

void Channel::setChannelOperator(Client * channelOperator)
{
	_channelOperator = channelOperator;
}

void	Channel::removeClient(Client* client)
{
	std::map<int, Client>::iterator it = _clients.find(client->getFd());
	_clients.erase(it);
}

void	Channel::removeClient(int fd)
{
	std::map<int, Client>::iterator it = _clients.find(fd);
	_clients.erase(it);
}
