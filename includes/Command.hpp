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
		virtual ~Command() = 0;

		//todo: 一旦引数をchar*型に、返り値をstringにしてる。今後はresponse構造体を返すようにしていく
		virtual std::string	execute(char* input) const = 0;

	private:
};

// ------------------------------------------------
// function
// ------------------------------------------------

Command::~Command(){}

#endif
