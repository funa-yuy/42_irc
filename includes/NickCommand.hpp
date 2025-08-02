#ifndef COMMAND_HPP
# define COMMAND_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>

// ------------------------------------------------
// class
// ------------------------------------------------

class NickCommand {
	public:
		~NickCommand();

		void	execute(std::string input) const;

	protected:

	private:
		NickCommand();
		NickCommand& operator=(const NickCommand& copy);
		NickCommand(const NickCommand& copy);
};

// ------------------------------------------------
// function
// ------------------------------------------------


#endif
