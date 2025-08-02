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
		Command(){};
		virtual ~Command() = 0;//純粋仮想関数

		virtual std::string	execute(char* input) const = 0; //純粋仮想関数

	protected:

	private:
		Command& operator=(const Command& copy);
		Command(const Command& copy);
};

// ------------------------------------------------
// function
// ------------------------------------------------


Command::~Command(){}

#endif
