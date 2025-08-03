#ifndef NICKCOMMAND_HPP
# define NICKCOMMAND_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>
#include "Command.hpp"

// ------------------------------------------------
// class
// ------------------------------------------------

class NickCommand : public Command {
	public:
		NickCommand(){};
		~NickCommand(){};

		std::string	execute(char* input) const {
			(void)input;
			// Todo: ここにコマンドの処理書く
			return (":servername Hi! NICK!\r\n");
		}

	private:
};

// ------------------------------------------------
// function
// ------------------------------------------------


#endif
