#include "Channel.hpp"

Channel::Channel() {}

Channel::Channel(std::string name, int createdBy) {
	setName(name);
	addClientFd(createdBy);
	setChannelOperatorFds(createdBy);
}

std::string Channel::getName() const
{
	return (_name);
}

const std::set<int>& Channel::getClientFds() const
{
	return (_clientFds);
}

std::string Channel::getTopic() const
{
	return (_topic);
}

const std::set<int>& Channel::getChannelOperatorFds() const
{
	return (_channelOperatorFds);
}

void Channel::setName(std::string name)
{
	_name = name;
}

void Channel::addClientFd(int fd)
{
	_clientFds.insert(fd);
}

void Channel::setTopic(std::string topic)
{
	_topic = topic;
}

void Channel::setChannelOperatorFds(int fd)
{
	if (_clientFds.find(fd) != _clientFds.end())
		_channelOperatorFds.insert(fd);
}

void	Channel::removeClientFd(int fd)
{
	std::set<int>::iterator it = _clientFds.find(fd);
	if (it == _clientFds.end())
		return ;
	_channelOperatorFds.erase(fd);//todo: オペレーターが全員退出した場合の処理を考える。
	_clientFds.erase(it);
}
