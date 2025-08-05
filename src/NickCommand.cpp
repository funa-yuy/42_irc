#include "../includes/NickCommand.hpp"


NickCommand::NickCommand() {}

NickCommand::~NickCommand() {}

// void	updateDatabase(char* input) {
// 	Database	data;

// 	//引数のfdからclientを特定。
// 	Client	target =  data.getClient(input.fd);
// 	target.setNickname = input.nick;
// }

const t_response	NickCommand::execute(char* input) const {
	(void)input;
	t_response	res;

	//todo: Databaseクラスを更新する処理を書く
	// updateDatabase(input);

	res.msg = ":servername Hi! NICK!!!\r\n";

	//todo: メッセージ送信対象のClientのfdを格納する
	// res.fds = ~~;
	//todo: send()のフラグを格納する
	// res.send_flag = ~~;

	return (res);
}
