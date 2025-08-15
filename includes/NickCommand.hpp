#ifndef NICKCOMMAND_HPP
# define NICKCOMMAND_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>
#include "Command.hpp"
#include "Database.hpp"

// ------------------------------------------------
// class
// ------------------------------------------------

class NickCommand : public Command {
	public:
		NickCommand();
		~NickCommand();

		const t_response	execute(const t_parsed& input) const;

	private:

};

// ------------------------------------------------
// function
// ------------------------------------------------


#endif
