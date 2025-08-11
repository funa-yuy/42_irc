#include "PassCommand.hpp"

PassCommand::PassCommand() {}

PassCommand::~PassCommand() {}

Command* PassCommand::createPassCommand() const {
	return (new PassCommand());
}

//updataDatabase()と、is_validCmd()もプライベートな純粋仮想関数にしてもいいのでは？
//また、数値のエラー検出もメンバ関数にしてもいいかも
void	updataDatabase(const t_parserd& input) {
	Database	data;
	Client		sender_client = data.getClient(input.sender_fd);

	sender_client.setIslogin(true);
}

bool	is_validCmd(const t_parserd& input, t_response* res) {
	Database	data;
	Client		sender_client = data.getClient(input.sender_fd);

	if (input.option.size() < 1)//引数が無効
	{
		//ERR_NEEDMOREPARAMS 461
		res->reply = ":servername Hi! PASS!!!NG!\r\n";
		res->target_fds[0] = input.sender_fd;
		res->send_flag = 0;
		return(false);
	}
	else if (sender_client.getIsLogin())//登録済みのクライアントが実行した
	{
		//ERR_ALREADYREGISTERED 462
		res->reply = ":servername Hi! PASS!!!NG!\r\n";
		res->target_fds[0] = input.sender_fd;
		res->send_flag = 0;
		return(false);
	}
	else if (data.getPassword() != input.option[0])//パスワードが正しくない
	{
		// ERR_PASSWDMISMATCH 464
		res->reply = ":servername Hi! PASS!!!NG!\r\n";
		res->target_fds[0] = input.sender_fd;
		res->send_flag = 0;
		return(false);
	}
	return(true);
}

const t_response	PassCommand::execute(const t_parserd& input) const {
	t_response	res;

	if (!is_validCmd(input, &res))
		return (res);

	res.reply = ":servername Hi! PASS!!!NG!\r\n";
	res.target_fds[0] = input.sender_fd;
	res.send_flag = 0;

	updataDatabase(input);
	return (res);
}
