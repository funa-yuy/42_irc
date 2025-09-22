#ifndef DATABASE_HPP
# define DATABASE_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <map>
#include <vector>
#include "Client.hpp"
#include "Channel.hpp"
#include "Utils.hpp"


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
	void			addChannel(Channel& channel);
	void			removeChannel(std::string& name);

	Client *			getClient(int fd);
	Client const *		getClient(int fd) const;
	Client *			getClient(std::string & nickname);
	Client const *		getClient(const std::string & nickname);
	Channel const *		getChannel(const std::string& name) const;
	Channel *			getChannel(const std::string& name);
	std::vector<std::string>	getAllChannelNames() const;
	const std::string&	getPassword() const;
	std::map<int, Client>&		getAllClient();

private:

	std::string	_password;

	std::map<int, Client>			_clients;// fd→Client、 ユーザ用のmapコンテナ、fdとClientオブジェクトのセットで管理
	std::map<std::string, Channel>	_channels;// name→Channel、チャンネル用のmapコンテナ、チャンネル名とChannelオブジェクトのセットで管理

};

// ------------------------------------------------
// function
// ------------------------------------------------


#endif
