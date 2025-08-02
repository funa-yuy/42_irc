#ifndef CHANNEL_HPP
# define CHANNEL_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>
#include "Client.hpp"

// ------------------------------------------------
// class
// ------------------------------------------------

class Channel {
	public:
		std::string	getName() const;
		std::vector<Client *>	getClients() const;
		Client*	getChannelOperator() const;
		std::string	getTopic() const;

	private:
		std::string				name;
		std::vector<Client *>	clients;
		Client*					channelOperator;
		std::string				topic;
	// ...他に必要に応じて（モードや制限など）


	//未使用の関数
	Channel();
	Channel& operator=(const Channel& copy);
	Channel(const Channel& copy);
	~Channel();
};

// ------------------------------------------------
// function
// ------------------------------------------------


#endif
