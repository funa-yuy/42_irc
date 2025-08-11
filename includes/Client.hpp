#ifndef CLIENT_HPP
# define CLIENT_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>
#include <sys/poll.h>

// ------------------------------------------------
// class
// ------------------------------------------------

class Client {

public:

	void			setClientFd(int fd);

	int				getFd(void) const;
	struct pollfd	getPfd(void) const;
	std::string		getNickname(void) const;
	std::string		getUsername(void) const;
	bool			getIsOperator(void) const;

private:

	struct pollfd	_pfd; // クライアントのfdなどが入っている
	std::string		_nickname;
	std::string		_username;
	bool			_isOperator;

	// ...他に必要に応じて（モードや制限など）
	bool			_passReceived;
	bool			_nickReceived;
	bool			_userReceived;

	// 
	bool			_registered;

};

// ------------------------------------------------
// function
// ------------------------------------------------

#endif
