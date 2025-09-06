#ifndef CHANNEL_HPP
# define CHANNEL_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>
#include "Client.hpp"
#include <map>


// ------------------------------------------------
// class
// ------------------------------------------------

class Channel {
	public:
		std::string	getName() const;
		std::map<int, Client>	getClients() const;
		Client*	getChannelOperator() const;
		std::string	getTopic() const;

		void	setName(std::string name);
		void	setChannelOperator(Client *channelOperator);
		void	setTopic(std::string);
		void	addClient(Client& client);

		void	removeClient(Client* client);
		void	removeClient(int fd);

	private:
		std::string				_name;
		// std::vector<Client *>	_clients;
		std::map<int, Client>	_clients;
		Client*					_channelOperator;
		std::string				_topic;
	// ...他に必要に応じて（モードや制限など）
};

// ------------------------------------------------
// function
// ------------------------------------------------


#endif
