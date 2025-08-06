#include "../includes/UserCommand.hpp"

UserCommand::UserCommand() {}

UserCommand::~UserCommand() {}

// void	updateDatabase(char* input) {
// 	Database	data;

// 	//引数のfdからclientを特定。
// 	Client	target =  data.getClient(input.fd);
// 	target.setUsername = input.name;
// }

const t_response	UserCommand::execute(const t_parserd& input) const {
	(void)input;
	t_response	res;

	//todo: Databaseクラスを更新する処理を書く
	// updateDatabase(input);

	res.reply = ":servername Hi! USER!!!\r\n";

	//todo: メッセージ送信対象のClientのfdを格納する
	// res.fds = ~~;
	//todo: send()のフラグを格納する
	// res.send_flag = ~~;

	return (res);
}
