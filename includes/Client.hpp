#ifndef CLIENT_HPP
# define CLIENT_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>


// ------------------------------------------------
// class
// ------------------------------------------------

class Client {
	public:
		int	getFd() const;
		std::string	getNickname() const;
		std::string	getUsername() const;
		bool	getIsOperator() const;

	private:
		int			fd;
		std::string	nickname;
		std::string	username;
		bool		isOperator;
	// ...他に必要に応じて（モードや制限など）


	//未使用の関数
	Client();
	Client& operator=(const Client& copy);
	Client(const Client& copy);
	~Client();
};

// ------------------------------------------------
// function
// ------------------------------------------------


#endif
