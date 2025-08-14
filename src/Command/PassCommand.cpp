#include "PassCommand.hpp"

PassCommand::PassCommand() {}

PassCommand::~PassCommand() {}

Command*	PassCommand::createPassCommand() const {
	return (new PassCommand());
}

bool	is_validCmd(const t_parserd& input, t_response* res) {
	Database	data;
	Client		sender_client = data.getClient(input.sender_fd);

	if (input.option.size() < 1)//ERR_NEEDMOREPARAMS 461 引数が無効
	{
		res->is_success = false;
		res->should_send = true;
		res->reply = ":servername 461 :Not enough parameters\r\n";
		res->target_fds[0] = input.sender_fd;
		res->send_flag = 0;
		return(false);
	}
	else if (sender_client.getIsOperator())//ERR_ALREADYREGISTRED 462 登録済みのクライアントが実行した
	{
		res->is_success = false;
		res->should_send = true;
		res->reply = ":servername 462 :Already registered\r\n";
		res->target_fds[0] = input.sender_fd;
		res->send_flag = 0;
		return(false);
	}
	else if (data.getPassword() != input.option[0])//ERR_PASSWDMISMATCH 464 パスワードが正しくない
	{
		res->is_success = false;
		res->should_send = true;
		res->reply = ":servername 464 :Password incorrect\r\n";
		res->target_fds[0] = input.sender_fd;
		res->send_flag = 0;
		return(false);
	}
	return(true);
}

//updataDatabase()と、is_validCmd()もプライベートな純粋仮想関数にしてもいいのでは？
//また、数値のエラー検出もメンバ関数にしてもいいかも
void	updataDatabase(const t_parserd& input) {
	Database	data;
	Client		sender_client = data.getClient(input.sender_fd);

	sender_client.setIsOperator(true);
}

const t_response	PassCommand::execute(const t_parserd& input) const {
	t_response	res;

	if (!is_validCmd(input, &res))
		return (res);

	res.is_success = true;
	res.should_send = false;

	updataDatabase(input);
	return (res);
}
