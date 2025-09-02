#include "Channel.hpp"

std::string Channel::getName() const
{
	return (_name);
}

std::vector<Client *> Channel::getClients() const
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
	_clients.push_back(&client);
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
	for (int i = 0; i < (int) _clients.size(); i ++)
	{
		if (_clients[i]->getNickname() == client->getNickname())
			_clients.erase(_clients.begin() + i);
	}
}

void	Channel::removeClient(std::string& nickname)
{
	for (int i = 0; i < (int) _clients.size(); i ++)
	{
		if (_clients[i]->getNickname() == nickname)
			_clients.erase(_clients.begin() + i);
	}
}
