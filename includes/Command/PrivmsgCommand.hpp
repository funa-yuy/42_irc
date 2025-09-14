#ifndef PRIVMSGCOMMAND_HPP
# define PRIVMSGCOMMAND_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>
#include "Command.hpp"
#include "Database.hpp"

#define EXCLAMATION_CHANNEL_LEN 6
#define SERVER_PREFIX ":ft.irc"
#define SERVER_NAME "ft.irc"

// ------------------------------------------------
// class
// ------------------------------------------------

class PrivmsgCommand : public Command {
	public:
		PrivmsgCommand();
		~PrivmsgCommand();
		static Command* createPrivmsgCommand();
		std::vector<t_response>	execute(const t_parsed& input, Database& db) const;
	
	private:
		bool				is_channel(std::string target) const;
		std::vector<int>	get_fd_ByNickname(std::string msgtarget, Database& db) const;
		std::vector<int>	get_fd_ByChannel(std::string target, Database& db) const;
		bool				is_belong_channel(const t_parsed& input, Database& db) const;
		std::vector<int>	get_target_fd(std::string target, Database& db) const;
		bool				is_validCmd(const t_parsed& input, t_response* res, Database& db) const;
		bool				return_false_set_reply(t_response *res, const t_parsed& input, std::string msg) const;
};

// ------------------------------------------------
// function
// ------------------------------------------------

#endif
