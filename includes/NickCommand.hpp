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
			return (":servername Hi! NICK!!!\r\n");
		}

	private:
		NickCommand& operator=(const NickCommand& copy);
		NickCommand(const NickCommand& copy);
};

// ------------------------------------------------
// function
// ------------------------------------------------


#endif
