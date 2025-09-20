#ifndef CHANNEL_HPP
# define CHANNEL_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>
#include <string>
#include <set>
#include <cctype>
#include "Client.hpp"
#include "Utils.hpp"

// ------------------------------------------------
// class
// ------------------------------------------------

class Channel {
	public:
		Channel();
		Channel(std::string name, int createdBy);
		std::string	getName() const;
		const std::set<int>&	getClientFds() const;
		const std::set<int>&	getChannelOperatorFds() const;
		void	setName(std::string name);
		void	setChannelOperatorFds(int fd);
		void	addClientFd(int fd);
		void	removeClientFd(int fd);

		bool		getTopicRestricted() const;
		void		setTopicRestricted(bool val);
		std::string	getTopic() const;
		void		setTopic(std::string);

		bool		getInviteOnly() const;
		void		setInviteOnly(bool val);
		const std::set<int>&	getInviteList() const;
		bool		isInvited(int fd) const;
		void		addInvite(int fd);
		void		removeInvite(int fd);
		void		clearInvites();

		bool		getHasKey() const;
		std::string	getKey() const;
		void		setKey(const std::string& key);
		void		clearKey();

		bool		getHasLimit() const;
		unsigned int	getLimit() const;
		void		setLimit(unsigned int limit);
		void		clearLimit();

	private:
		std::string				_name;
		std::set<int>			_clientFds;
		std::set<int>			_channelOperatorFds;
		bool					_topicRestricted;
		std::string				_topic;
		bool					_inviteOnly;
		std::set<int>			_inviteList;
		bool					_hasKey;
		std::string				_key;
		bool					_haslimit;
		unsigned int				_limit;
};

// ------------------------------------------------
// function
// ------------------------------------------------


#endif
