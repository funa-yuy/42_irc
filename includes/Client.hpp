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

class Client
{

public:

	Client();
	~Client();

	void				initializeClient(int fd);

	int					getFd(void) const;
	struct pollfd		getPfd(void) const;
	std::string &		getBuffer(void);
	std::string const &	getBuffer(void) const;

	void				setNickname(std::string nickname);
	std::string			getNickname(void) const;
	std::string			getUsername(void) const;
	std::string			getRealname(void) const;

	bool				getUserReceived(void) const;
	bool				getIsOperator(void) const;
	bool				getIsRegistered(void) const;
	
	void				setNickname(const std::string& nickname);
	void				setUsername(const std::string& username);
	void				setRealname(const std::string& realname);
	void				setUserReceived(bool val);
	void				setIsRegistered(bool val);

private:

	struct pollfd	_pfd; // クライアントのfdなどが入っている
	int				_fd;
	std::string		_buffer;

	std::string		_nickname;
	std::string		_username;
	std::string		_realname;

	bool			_passReceived;
	bool			_nickReceived;
	bool			_userReceived;

	bool			_isRegistered;
	bool			_isOperator;

};

// ------------------------------------------------
// function
// ------------------------------------------------

#endif
