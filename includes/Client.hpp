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
		bool	getIsRegistered() const;
		void	setIsRegistered(bool val);

	private:
		int			_fd;
		std::string	_nickname;
		std::string	_username;
		bool		_isOperator;
		bool		_isRegistered;
	// ...他に必要に応じて（モードや制限など）
};

// ------------------------------------------------
// function
// ------------------------------------------------


#endif
