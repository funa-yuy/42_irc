#ifndef JOINCOMMAND_HPP
# define JOINCOMMAND_HPP

// ------------------------------------------------
// include
// ------------------------------------------------

#include <iostream>
#include "Command.hpp"

struct s_join_item {
	std::string	channel;
	std::string	key;
};

// ------------------------------------------------
// class
// ------------------------------------------------

class JoinCommand : public Command {
	public:
		JoinCommand();
		~JoinCommand();
		static Command* createJoinCommand();
		std::vector<t_response>	execute(const t_parsed& input, Database& db) const;

	private:
		const std::vector<t_response>	executeJoin(const t_parsed& input, Database& db, std::vector<s_join_item> items) const;
		bool							is_validCmd(const t_parsed& input, t_response* res, Database& db, const s_join_item& item) const;
		std::vector<s_join_item>		parse_join_args(const t_parsed& input) const;
		void							updateDatabase(const t_parsed& input, Database& db, const s_join_item& item) const;
		t_response						makeJoinBroadcast(const t_parsed& input, Database& db, Channel* channel) const;
		t_response						makeRplTopic(const t_parsed& input, Channel* channel) const;
		t_response						makeRplNamreply(const t_parsed& input, Database& db, Channel* channel) const;
};

// ------------------------------------------------
// function
// ------------------------------------------------

#endif
