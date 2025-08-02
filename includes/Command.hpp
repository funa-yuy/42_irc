#ifndef COMMAND_HPP
# define COMMAND_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>

// ------------------------------------------------
// class
// ------------------------------------------------

class Command {
	public:
		virtual ~Command() = 0;//純粋仮想関数

		virtual void	execute(std::string input) const = 0; //純粋仮想関数

	protected:

	private:
		Command();
		Command& operator=(const Command& copy);
		Command(const Command& copy);
};

// ------------------------------------------------
// function
// ------------------------------------------------


#endif
