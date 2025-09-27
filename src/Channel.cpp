#include "Channel.hpp"

Channel::Channel()
: _createdAt(time(NULL))
, _topicRestricted(true)
, _topic("")
, _topicTime(0)
, _topicWho(-1)
, _inviteOnly(false)
, _hasKey(false)
, _haslimit(false)
, _limit(0)
{}

Channel::Channel(std::string name, int createdBy)
: _createdAt(time(NULL))
, _topicRestricted(true)
, _topic("")
, _topicTime(0)
, _topicWho(-1)
, _inviteOnly(false)
, _hasKey(false)
, _haslimit(false)
, _limit(0)
{
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
	_name = toLowerCase(name);
}

void Channel::addClientFd(int fd)
{
	_clientFds.insert(fd);
}

void Channel::setTopic(std::string topic)
{
	_topic = topic;
}

void	Channel::clearTopic()
{
	_topic.clear();
	_topicWho = -1;
	_topicTime = 0;
}

time_t		Channel::getTopicTime() const
{
	return (_topicTime);
}

void	Channel::setTopicTime(time_t val)
{
	_topicTime = val;
}

int		Channel::getTopicWho() const
{
	return (_topicWho);
}

void	Channel::setTopicWho(int fd)
{
	_topicWho = fd;
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
	_channelOperatorFds.erase(fd);
	_clientFds.erase(it);
}

void	Channel::removeChannelOperatorFd(int fd)
{
	_channelOperatorFds.erase(fd);
}

bool	Channel::isMember(int fd)
{
	std::set<int>::iterator	it = _clientFds.find(fd);
	if (it == _clientFds.end())
		return (false);
	return (true);
}

bool	Channel::isOperator(int fd)
{
	std::set<int>::iterator	it = _channelOperatorFds.find(fd);
	if (it == _channelOperatorFds.end())
		return (false);
	return (true);
}

time_t	Channel::getCreationTime() const
{
	return (_createdAt);
}

bool	Channel::getInviteOnly() const
{
	return (_inviteOnly);
}

void	Channel::setInviteOnly(bool val)
{
	_inviteOnly = val;
}

const std::set<int>& Channel::getInviteList() const
{
	return (_inviteList);
}

bool	Channel::isInvited(int fd) const
{
	return (_inviteList.find(fd) != _inviteList.end());
}

void	Channel::addInvite(int fd)
{
	_inviteList.insert(fd);
}

void	Channel::removeInvite(int fd)
{
	_inviteList.erase(fd);
}

void	Channel::clearInvites()
{
	_inviteOnly = false;
	_inviteList.clear();
}

bool	Channel::getTopicRestricted() const
{
	return (_topicRestricted);
}

void	Channel::setTopicRestricted(bool val)
{
	_topicRestricted = val;
}

bool	Channel::getHasKey() const
{
	return (_hasKey);
}

std::string Channel::getKey() const
{
	return (_key);
}

void	Channel::setKey(const std::string& key)
{
	_hasKey = true;
	_key = key;
}

void	Channel::clearKey()
{
	_hasKey = false;
	_key.clear();
}

bool	Channel::getHasLimit() const
{
	return (_haslimit);
}

unsigned int Channel::getLimit() const
{
	return (_limit);
}

void	Channel::setLimit(unsigned int limit)
{
	_haslimit = true;
	_limit = limit;
}

void	Channel::clearLimit()
{
	_haslimit = false;
	_limit = 0;
}
