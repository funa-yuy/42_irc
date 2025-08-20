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
		NickCommand();
		~NickCommand();

		const t_response	execute(const t_parsed& input, Database& db) const;
		void				set_err_res(t_response *res,
										const t_parsed& input,
										std::string errmsg) const;
		bool				is_special_char(char c) const;

	private:

};

// ------------------------------------------------
// function
// ------------------------------------------------


#endif
