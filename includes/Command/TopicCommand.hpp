#ifndef TOPICCOMMAND_HPP
# define TOPICCOMMAND_HPP

#include "Command.hpp"

class TopicCommand : public Command
{

public:

	TopicCommand();
	~TopicCommand();

	static Command*			createTopicCommand();
	std::vector<t_response>	execute(const t_parsed& input, Database& db) const;

private:

	bool		isValidCmd(const t_parsed & input, t_response* res, Database & db) const;
	t_response	makeRplBroadcast(Client& client, Channel& ch) const;
	t_response	makeRplNotopic(Client& client, const std::string& chName) const;
	t_response	makeRplTopic(Client& client, Channel& ch) const;
	t_response	makeRplTopicWhoTime(Client& client, Channel& ch) const;
};

#endif
