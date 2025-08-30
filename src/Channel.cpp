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

void Channel::setClients(std::vector<Client *> clients)
{
	_clients = clients;
}

void Channel::setTopic(std::string topic)
{
	_topic = topic;
}

void Channel::setChannelOperator(Client * channelOperator)
{
	_channelOperator = channelOperator;
}
