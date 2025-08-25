#ifndef NICKCOMMAND_HPP
# define NICKCOMMAND_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>
#include "Command.hpp"

# define NICKLEN 9

// ------------------------------------------------
// class
// ------------------------------------------------

class NickCommand : public Command {
	public:
		NickCommand();
		~NickCommand();

		const t_response	execute(const t_parsed& input, Database& db) const;
		static Command* createNickCommand();

	private:
		void				set_err_res(t_response *res,
										const t_parsed& input,
										std::string errmsg) const;
		bool				is_special_char(char c) const;
		bool	is_validCmd(const t_parsed& input, t_response* res, Database& db) const;

};

// ------------------------------------------------
// function
// ------------------------------------------------


#endif
