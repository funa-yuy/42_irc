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
		std::string	getTopic() const;

		void	setName(std::string name);
		void	setChannelOperatorFds(int fd);
		void	setTopic(std::string);
		void	addClientFd(int fd);
		void	removeClientFd(int fd);

		bool	isMember(int fd);

	private:
		std::string				_name;
		std::set<int>			_clientFds;
		std::set<int>			_channelOperatorFds;
		std::string				_topic;
	// ...他に必要に応じて（モードや制限など）
};

// ------------------------------------------------
// function
// ------------------------------------------------


#endif
