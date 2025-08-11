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

	Client();
	~Client();

	void				initializeClient(int fd);

	int					getFd(void) const;
	struct pollfd		getPfd(void) const;
	std::string &		getBuffer(void);
	std::string const &	getBuffer(void) const;

	std::string			getNickname(void) const;
	std::string			getUsername(void) const;
	bool				getIsOperator(void) const;

private:

	struct pollfd	_pfd; // クライアントのfdなどが入っている
	int				_fd;
	std::string		_buffer;

	std::string		_nickname;
	std::string		_username;
	
	// ...他に必要に応じて（モードや制限など）
	bool			_passReceived;
	bool			_nickReceived;
	bool			_userReceived;
	
	// 
	bool			_isRegistered;
	bool			_isOperator;

};

// ------------------------------------------------
// function
// ------------------------------------------------

#endif
