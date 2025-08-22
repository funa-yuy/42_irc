#ifndef DATABASE_HPP
# define DATABASE_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <map>
#include "Client.hpp"
#include "Channel.hpp"


// ------------------------------------------------
// class
// ------------------------------------------------

class Database
{

public:

	Database(std::string password);
	~Database();

	Client *		addClient(int fd);
	void			removeClient(int fd);

	Client *		getClient(int fd);
	Client const *	getClient(int fd) const;
	Channel *		getChannel(std::string & name) const;
	const std::string&	getPassword() const;
	std::map<int, Client>& getAllClient();

private:

	std::string	_password;

	std::map<int, Client>			_clients;// fd→Client、 ユーザ用のmapコンテナ、fdとClientオブジェクトのセットで管理
	std::map<std::string, Channel>	_channels;// name→Channel、チャンネル用のmapコンテナ、チャンネル名とChannelオブジェクトのセットで管理

};

// ------------------------------------------------
// function
// ------------------------------------------------


#endif
