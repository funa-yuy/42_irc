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

class Database {
	public:
		Client*	getClient(int fd) const;
		Channel*	getChannel(std::string &name) const;

	private:
		std::map<int, Client> _clients;// fd→Client、 ユーザ用のmapコンテナ、fdとClientオブジェクトのセットで管理
		std::map<std::string, Channel> _channels;// name→Channel、チャンネル用のmapコンテナ、チャンネル名とChannelオブジェクトのセットで管理

	//未使用の関数
		Database();
		Database& operator=(const Database& copy);
		Database(const Database& copy);
		~Database();
};

// ------------------------------------------------
// function
// ------------------------------------------------


#endif
