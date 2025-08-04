#include "../includes/PassCommand.hpp"

PassCommand::PassCommand() {}

PassCommand::~PassCommand() {}

const t_response	PassCommand::execute(char* input) const {
	(void)input;
	t_response	res;
	// Database	data;

	//todo: ircサーバー実行時に引数で与えられたPasswordをどこに保存するか？
	// if (data.getPassword == input.msg)
	// 	res.msg = ":servername Hi! PASS!!!OK!\r\n";
	// else
		res.msg = ":servername Hi! PASS!!!NG!\r\n";


	//todo: PASSコマンドに関しては、送信元のClientのみ返信でいいのでは？
	// res.fds = input.fd;
	//todo: send()のフラグを格納する
	// res.send_flag = ~~;

	return (res);
}
