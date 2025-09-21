#include "Command/InviteCommand.hpp"

InviteCommand::InviteCommand() {}

InviteCommand::~InviteCommand() {}

Command *	InviteCommand::createInviteCommand() { return (new InviteCommand()); }

std::vector<t_response>	InviteCommand::execute(const t_parsed & input, Database & db) const
{
	std::vector<t_response>	responses;
	t_response				res;

	res.is_success = false;
	res.should_disconnect = false;
	if (!isValidCmd(input, res, db))
	{
		responses.push_back(res);
		return (responses);
	}

	Client *	inviter = db.getClient(input.client_fd);
	const std::string	inviteeNick = input.args[0];
	Client *	invitee = db.getClient(inviteeNick);
	const std::string	chName = input.args[1];
	Channel *	ch = db.getChannel(chName);

	if (!invitee || !inviter)
		return (responses);

	if (ch)
		ch->addInvite(invitee->getFd());

	responses.push_back(makeRplInviting(*inviter, *invitee, chName));
	responses.push_back(makeInviteLine(*inviter, *invitee, chName));

	return (responses);
}

bool	InviteCommand::isValidCmd(const t_parsed & input, t_response & res, Database & db) const
{
	res.should_send = true;
	res.target_fds.clear();
	res.target_fds.push_back(input.client_fd);

	Client *	inviter = db.getClient(input.client_fd);
	if (!inviter)
	{
		res.should_send = false;
		return (false);
	}

	if (input.args.size() < 2)
	{
		res.reply = ":ft.irc 461 "
					+ inviter->getNickname()
					+ " INVITE :Not enough parameters\r\n";
		return (false);
	}

	const std::string	inviteeNick = input.args[0];
	const std::string	chName = input.args[1];

	Client *	invitee = db.getClient(inviteeNick);
	if (!invitee)
	{
		res.reply = ":ft.irc 401 "
					+ inviter->getNickname() + " " + inviteeNick
					+ " :No such nick/channel\r\n";
		return (false);
	}

	Channel *	ch = db.getChannel(chName);
	if (ch)
	{
		if (!ch->isMember(inviter->getFd()))
		{
			res.reply = ":ft.irc 442 "
						+ inviter->getNickname() + " " + chName
						+ " :You're not on that channel\r\n";
			return (false);
		}
		if (!ch->isOperator(inviter->getFd()))
		{
			res.reply = ":ft.irc 482 "
						+ inviter->getNickname() + " " + chName
						+ " :You're not channel operator\r\n";
			return (false);
		}
		if (ch->isMember(invitee->getFd()))
		{
			res.reply = ":ft.irc 443 "
						+ inviter->getNickname() + " " + invitee->getNickname() + " " + chName
						+ " :is already on channel\r\n";
			return (false);
		}
	}

	res.is_success = true;
	return (true);
}

t_response	InviteCommand::makeRplInviting(Client & inviter, Client & invitee, const std::string & chName) const
{
	t_response	res;

	res.should_disconnect = false;
	res.should_send = true;

	res.reply = ":ft.irc 341 " + inviter.getNickname() + " " + invitee.getNickname() + " " + chName + "\r\n";
	res.target_fds.push_back(inviter.getFd());

	res.is_success = true;
	return (res);
}

t_response	InviteCommand::makeInviteLine(Client & inviter, Client & invitee, const std::string & chName) const
{
	t_response	res;

	res.should_disconnect = false;
	res.should_send = true;

	res.reply = ":" + inviter.getNickname() + "!" + inviter.getUsername() + "@ft.irc"
				+ " INVITE " + invitee.getNickname() + " " + chName + "\r\n";
	res.target_fds.push_back(invitee.getFd());

	res.is_success = true;
	return (res);
}
