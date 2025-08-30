#ifndef CHANNEL_HPP
# define CHANNEL_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>
#include "Client.hpp"
#include <vector>


// ------------------------------------------------
// class
// ------------------------------------------------

class Channel {
	public:
		std::string	getName() const;
		std::vector<Client *>	getClients() const;
		Client*	getChannelOperator() const;
		std::string	getTopic() const;
		void	setName(std::string name);
		void	setClients(std::vector<Client *> clients);
		void	setChannelOperator(Client *channelOperator);
		void	setTopic(std::string);

	private:
		std::string				_name;
		std::vector<Client *>	_clients;
		Client*					_channelOperator;
		std::string				_topic;
	// ...他に必要に応じて（モードや制限など）
};

// ------------------------------------------------
// function
// ------------------------------------------------


#endif
