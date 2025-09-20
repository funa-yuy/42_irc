#include <cassert>
#include <iostream>
#include <algorithm>

#include "Database.hpp"
#include "Command/JoinCommand.hpp"

static t_parsed makeInput(const std::string& cmd, int fd, const std::vector<std::string>& args) {
	t_parsed in;
	in.cmd = cmd;
	in.client_fd = fd;
	in.args = args;
	return in;
}

static void test_success() {
	Database db("password");
	JoinCommand join;

	// 正常: 引数にチャンネルのみ
	{
		int	fd = 4;
		Client* cl4 = db.addClient(fd);
		cl4->setNickname("nick4");

		std::vector<std::string> args;
		args.push_back("#hoge");

		t_parsed in = makeInput("JOIN", fd, args);
		std::vector<t_response> res = join.execute(in, db);

		assert(res.size() == 3);
		// JOIN
		assert(res[0].is_success == true);
		assert(res[0].should_send == true);
		assert(res[0].should_disconnect == false);
		assert(res[0].reply.find("nick4 has joined #hoge") != std::string::npos);
		assert(res[0].target_fds.size() == 1 && res[0].target_fds[0] == fd);
		// 332 RPL_TOPIC
		assert(res[1].is_success == true);
		assert(res[1].should_send == true);
		assert(res[1].should_disconnect == false);
		assert(res[1].reply.find(" 332 Topic for #hoge :") != std::string::npos);
		assert(res[1].target_fds.size() == 1 && res[1].target_fds[0] == fd);
		// 353 RPL_NAMREPLY
		assert(res[2].is_success == true);
		assert(res[2].should_send == true);
		assert(res[2].should_disconnect == false);
		assert(res[2].reply.find(" 353 =#hoge ") != std::string::npos);
		assert(res[2].target_fds.size() == 1 && res[2].target_fds[0] == fd);
	}

	// 正常: 既存メンバーがいるチャンネルにJOIN → JOIN通知のfdsが全員分含む
	{
		int existing_fd = 4; // 上のケースで #hoge にいる既存メンバー
		int joiner_fd = 40;
		Client* cl40 = db.addClient(joiner_fd);
		cl40->setNickname("nick40");

		std::vector<std::string> args;
		args.push_back("#hoge");
		t_parsed in = makeInput("JOIN", joiner_fd, args);
		std::vector<t_response> res = join.execute(in, db);

		// 3通（JOIN, 332, 353）
		assert(res.size() == 3);
		// JOIN通知のfdsに既存(4)と新規(40)の両方が含まれているか
		const std::vector<int>& fds = res[0].target_fds;
		assert(fds.size() == 2);
		assert(std::find(fds.begin(), fds.end(), existing_fd) != fds.end());
		assert(std::find(fds.begin(), fds.end(), joiner_fd) != fds.end());
		assert(res[0].reply.find("nick40 has joined #hoge") != std::string::npos);
	}

	// 正常: 引数に複数のチャンネルと複数のキー
	{
		int	fd = 5;
		Client* cl5 = db.addClient(fd);
		cl5->setNickname("nick5");

		std::vector<std::string> args;
		args.push_back("&hoge,&fuga");
		args.push_back("hogeKey,fugaKey");

		t_parsed in = makeInput("JOIN", fd, args);
		std::vector<t_response> res = join.execute(in, db);

		assert(res.size() == 6);
		// 1つ目チャンネル: JOIN通知, 332, 353
		assert(res[0].reply.find("nick5 has joined &hoge") != std::string::npos);
		assert(res[0].target_fds.size() == 1 && res[0].target_fds[0] == fd);
		assert(res[1].reply.find(" 332 Topic for &hoge :") != std::string::npos);
		assert(res[1].target_fds.size() == 1 && res[1].target_fds[0] == fd);
		assert(res[2].reply.find(" 353 ") != std::string::npos);
		assert(res[2].target_fds.size() == 1 && res[2].target_fds[0] == fd);
		// 2つ目チャンネル: JOIN通知, 332, 353
		assert(res[3].reply.find("nick5 has joined &fuga") != std::string::npos);
		assert(res[3].target_fds.size() == 1 && res[3].target_fds[0] == fd);
		assert(res[4].reply.find(" 332 Topic for &fuga :") != std::string::npos);
		assert(res[4].target_fds.size() == 1 && res[4].target_fds[0] == fd);
		assert(res[5].reply.find(" 353 ") != std::string::npos);
		assert(res[5].target_fds.size() == 1 && res[5].target_fds[0] == fd);
	}

	// 正常: 引数に2つのチャンネルと1つのキー
	{
		int	fd = 6;
		Client* cl6 = db.addClient(fd);
		cl6->setNickname("nick6");

		std::vector<std::string> args;
		args.push_back("+hoge,+fuga");
		args.push_back("hogeKey");

		t_parsed in = makeInput("JOIN", fd, args);
		std::vector<t_response> res = join.execute(in, db);

		assert(res.size() == 6);
		// 1つ目チャンネル: JOIN通知, 332, 353
		assert(res[0].reply.find("nick6 has joined +hoge") != std::string::npos);
		assert(res[1].reply.find(" 332 Topic for +hoge :") != std::string::npos);
		assert(res[2].reply.find(" 353 ") != std::string::npos);
		// 2つ目チャンネル: JOIN通知, 332, 353
		assert(res[3].reply.find("nick6 has joined +fuga") != std::string::npos);
		assert(res[4].reply.find(" 332 Topic for +fuga :") != std::string::npos);
		assert(res[5].reply.find(" 353 ") != std::string::npos);
	}

	// 正常: 引数に1つのチャンネルと2つのキー(2つ目のキーは無視される)
	{
		int	fd = 7;
		Client* cl7 = db.addClient(fd);
		cl7->setNickname("nick7");

		std::vector<std::string> args;
		args.push_back("!ABC12hoge");
		args.push_back("hogeKey,fugaKey");

		t_parsed in = makeInput("JOIN", fd, args);
		std::vector<t_response> res = join.execute(in, db);

		assert(res.size() == 3);
		// JOIN
		assert(res[0].is_success == true);
		assert(res[0].should_send == true);
		assert(res[0].reply.find("nick7 has joined !abc12hoge") != std::string::npos);
		assert(res[0].target_fds.size() == 1 && res[0].target_fds[0] == fd);
		// 332 RPL_TOPIC
		assert(res[1].is_success == true);
		assert(res[1].should_send == true);
		assert(res[1].reply.find(" 332 Topic for !abc12hoge :") != std::string::npos);
		assert(res[1].target_fds.size() == 1 && res[1].target_fds[0] == fd);
		// 353 RPL_NAMREPLY
		assert(res[2].is_success == true);
		assert(res[2].should_send == true);
		assert(res[2].reply.find(" 353 ") != std::string::npos);
		assert(res[2].target_fds.size() == 1 && res[2].target_fds[0] == fd);
	}

	// 正常: キーが一致
	{
		int	op_fd = 80;
		Client* op = db.addClient(op_fd);
		op->setNickname("op80");

		Channel ch("#keyok", op_fd);
		ch.setKey("secret");
		db.addChannel(ch);

		int	joiner_fd = 81;
		Client* cl = db.addClient(joiner_fd);
		cl->setNickname("nick81");

		std::vector<std::string> args;
		args.push_back("#keyok");
		args.push_back("secret");
		t_parsed in = makeInput("JOIN", joiner_fd, args);
		std::vector<t_response> res = join.execute(in, db);

		assert(res.size() == 3);
		const std::vector<int>& fds = res[0].target_fds;
		assert(std::find(fds.begin(), fds.end(), op_fd) != fds.end());
		assert(std::find(fds.begin(), fds.end(), joiner_fd) != fds.end());
		assert(res[0].reply.find("nick81 has joined #keyok") != std::string::npos);
	}

	// 正常: 招待されているクライアント
	{
		int	op_fd = 82;
		Client* op = db.addClient(op_fd);
		op->setNickname("op82");

		int	joiner_fd = 83;
		Client* cl = db.addClient(joiner_fd);
		cl->setNickname("nick83");

		Channel ch("#inviteok", op_fd);
		ch.setInviteOnly(true);
		ch.addInvite(joiner_fd);
		db.addChannel(ch);

		std::vector<std::string> args;
		args.push_back("#inviteok");
		t_parsed in = makeInput("JOIN", joiner_fd, args);
		std::vector<t_response> res = join.execute(in, db);

		assert(res.size() == 3);
		const std::vector<int>& fds = res[0].target_fds;
		assert(std::find(fds.begin(), fds.end(), op_fd) != fds.end());
		assert(std::find(fds.begin(), fds.end(), joiner_fd) != fds.end());
		assert(res[0].reply.find("nick83 has joined #inviteok") != std::string::npos);
	}

	// 正常: チャンネル参加可能人数を超えていない
	{
		int	op_fd = 84;
		Client* op = db.addClient(op_fd);
		op->setNickname("op84");

		Channel ch("#limitok", op_fd);
		ch.setLimit(2);
		db.addChannel(ch);

		int	joiner_fd = 85;
		Client* cl = db.addClient(joiner_fd);
		cl->setNickname("nick85");

		std::vector<std::string> args;
		args.push_back("#limitok");
		t_parsed in = makeInput("JOIN", joiner_fd, args);
		std::vector<t_response> res = join.execute(in, db);

		assert(res.size() == 3);
		const std::vector<int>& fds = res[0].target_fds;
		assert(std::find(fds.begin(), fds.end(), op_fd) != fds.end());
		assert(std::find(fds.begin(), fds.end(), joiner_fd) != fds.end());
		assert(res[0].reply.find("nick85 has joined #limitok") != std::string::npos);
	}
}

static void test_err_461_needmoreparams() {
	Database db("password");
	JoinCommand join;

	int fd = 10;
	db.addClient(fd);

	std::vector<std::string> args; // argsが空
	t_parsed in = makeInput("JOIN", fd, args);
	std::vector<t_response> res = join.execute(in, db);

	assert(res.size() == 1);
	assert(res[0].is_success == false);
	assert(res[0].should_send == true);
	assert(res[0].reply.find("461") != std::string::npos);
	assert(res[0].target_fds.size() == 1 && res[0].target_fds[0] == fd);
}

static void test_err_403_nosuchchannel() {
	Database db("password");
	JoinCommand join;

	{
		int fd = 11;
		db.addClient(fd);

		std::vector<std::string> args;
		args.push_back("foo"); // 無効なチャンネル名: "#", "&", "+", "!" で開始していない
		t_parsed in = makeInput("JOIN", fd, args);
		std::vector<t_response> res = join.execute(in, db);
		assert(res.size() == 1);
		assert(res[0].is_success == false);
		assert(res[0].should_send == true);
		assert(res[0].reply.find("403 foo") != std::string::npos);
		assert(res[0].target_fds.size() == 1 && res[0].target_fds[0] == fd);
	}

	{
		int fd = 12;
		db.addClient(fd);

		std::vector<std::string> args;
		args.push_back("f  oo"); // 無効なチャンネル名: 空白が含まれている
		t_parsed in = makeInput("JOIN", fd, args);
		std::vector<t_response> res = join.execute(in, db);
		assert(res.size() == 1);
		assert(res[0].is_success == false);
		assert(res[0].should_send == true);
		assert(res[0].reply.find("403 f  oo") != std::string::npos);
		assert(res[0].target_fds.size() == 1 && res[0].target_fds[0] == fd);
	}

	{
		int fd = 13;
		db.addClient(fd);

		std::vector<std::string> args;
		args.push_back("foo,,");//チャンネル名が空
		args.push_back("fookey,notkey");
		t_parsed in = makeInput("JOIN", fd, args);
		std::vector<t_response> res = join.execute(in, db);
		assert(res.size() == 2);
		assert(res[0].is_success == false);
		assert(res[0].should_send == true);
		assert(res[0].reply.find("403 ") != std::string::npos);
		assert(res[0].target_fds.size() == 1 && res[0].target_fds[0] == fd);
	}

	{
		int fd = 14;
		db.addClient(fd);

		std::vector<std::string> args;
		args.push_back("#");//チャンネル名が空
		t_parsed in = makeInput("JOIN", fd, args);
		std::vector<t_response> res = join.execute(in, db);
		assert(res.size() == 1);
		assert(res[0].is_success == false);
		assert(res[0].should_send == true);
		assert(res[0].reply.find("403 ") != std::string::npos);
		assert(res[0].target_fds.size() == 1 && res[0].target_fds[0] == fd);
	}

	{
		int fd = 15;
		db.addClient(fd);

		std::vector<std::string> args;
		args.push_back("#foo:"); // `:`の後に何もない
		t_parsed in = makeInput("JOIN", fd, args);
		std::vector<t_response> res = join.execute(in, db);
		assert(res.size() == 1);
		assert(res[0].is_success == false);
		assert(res[0].should_send == true);
		assert(res[0].reply.find("403 ") != std::string::npos);
		assert(res[0].target_fds.size() == 1 && res[0].target_fds[0] == fd);
	}

	{
		int fd = 16;
		db.addClient(fd);

		std::vector<std::string> args;
		args.push_back("#foo:foo2:foo3"); // `:`が2つ以上ある
		t_parsed in = makeInput("JOIN", fd, args);
		std::vector<t_response> res = join.execute(in, db);
		assert(res.size() == 1);
		assert(res[0].is_success == false);
		assert(res[0].should_send == true);
		assert(res[0].reply.find("403 ") != std::string::npos);
		assert(res[0].target_fds.size() == 1 && res[0].target_fds[0] == fd);
	}
}

static void test_err_476_badchanmask() {
	Database db("password");
	JoinCommand join;

	{
		int fd = 16;
		db.addClient(fd);

		std::vector<std::string> args;
		args.push_back("!ABC");//5文字以下
		t_parsed in = makeInput("JOIN", fd, args);
		std::vector<t_response> res = join.execute(in, db);
		assert(res.size() == 1);
		assert(res[0].is_success == false);
		assert(res[0].should_send == true);
		assert(res[0].reply.find("476 ") != std::string::npos);
		assert(res[0].target_fds.size() == 1 && res[0].target_fds[0] == fd);
	}

	{
		int fd = 17;
		db.addClient(fd);

		std::vector<std::string> args;
		args.push_back("!abc123");//小文字になってる
		t_parsed in = makeInput("JOIN", fd, args);
		std::vector<t_response> res = join.execute(in, db);
		assert(res.size() == 1);
		assert(res[0].is_success == false);
		assert(res[0].should_send == true);
		assert(res[0].reply.find("476 ") != std::string::npos);
		assert(res[0].target_fds.size() == 1 && res[0].target_fds[0] == fd);
	}

	{
		int fd = 18;
		db.addClient(fd);

		std::vector<std::string> args;
		args.push_back("!ABC12");//5文字の後にチャンネル名がない
		t_parsed in = makeInput("JOIN", fd, args);
		std::vector<t_response> res = join.execute(in, db);
		assert(res.size() == 1);
		assert(res[0].is_success == false);
		assert(res[0].should_send == true);
		assert(res[0].reply.find("476 ") != std::string::npos);
		assert(res[0].target_fds.size() == 1 && res[0].target_fds[0] == fd);
	}
}

static void test_err_471_channelisfull() {
	Database db("password");
	JoinCommand join;

	int op_fd = 50;
	Client* op = db.addClient(op_fd);
	op->setNickname("op50");

	// 事前にチャンネルを作成しlimit=1に設定（opのみ入室可）
	Channel ch("#full", op_fd);
	ch.setLimit(1);
	db.addChannel(ch);

	// 新規クライアントがJOINすると満員エラー 471
	int joiner_fd = 51;
	Client* cl = db.addClient(joiner_fd);
	cl->setNickname("nick51");

	std::vector<std::string> args;
	args.push_back("#full");
	t_parsed in = makeInput("JOIN", joiner_fd, args);
	std::vector<t_response> res = join.execute(in, db);

	assert(res.size() == 1);
	assert(res[0].is_success == false);
	assert(res[0].should_send == true);
	assert(res[0].reply.find(" 471 #full ") != std::string::npos);
	assert(res[0].target_fds.size() == 1 && res[0].target_fds[0] == joiner_fd);
}

static void test_err_473_inviteonly() {
	Database db("password");
	JoinCommand join;

	int op_fd = 60;
	Client* op = db.addClient(op_fd);
	op->setNickname("op60");

	// 招待制チャンネル作成（招待なし）
	Channel ch("#invite", op_fd);
	ch.setInviteOnly(true);
	db.addChannel(ch);

	int joiner_fd = 61;
	Client* cl = db.addClient(joiner_fd);
	cl->setNickname("nick61");

	std::vector<std::string> args;
	args.push_back("#invite");
	t_parsed in = makeInput("JOIN", joiner_fd, args);
	std::vector<t_response> res = join.execute(in, db);

	assert(res.size() == 1);
	assert(res[0].is_success == false);
	assert(res[0].should_send == true);
	assert(res[0].reply.find(" 473 #invite ") != std::string::npos);
	assert(res[0].target_fds.size() == 1 && res[0].target_fds[0] == joiner_fd);
}

static void test_err_475_badchannelkey() {
	Database db("password");
	JoinCommand join;

	int op_fd = 70;
	Client* op = db.addClient(op_fd);
	op->setNickname("op70");

	// キー付きチャンネル作成
	Channel ch("#keychan", op_fd);
	ch.setKey("secret");
	db.addChannel(ch);

	int joiner_fd = 71;
	Client* cl = db.addClient(joiner_fd);
	cl->setNickname("nick71");

	// 間違ったキーでJOIN
	std::vector<std::string> args;
	args.push_back("#keychan");
	args.push_back("wrongkey");
	t_parsed in = makeInput("JOIN", joiner_fd, args);
	std::vector<t_response> res = join.execute(in, db);

	assert(res.size() == 1);
	assert(res[0].is_success == false);
	assert(res[0].should_send == true);
	assert(res[0].reply.find(" 475 #keychan ") != std::string::npos);
	assert(res[0].target_fds.size() == 1 && res[0].target_fds[0] == joiner_fd);
}

int main() {
	test_success();//正常
	test_err_461_needmoreparams();// エラー: ERR_NEEDMOREPARAMS 461 引数が無い
	test_err_403_nosuchchannel();// エラー: ERR_NOSUCHCHANNEL 403 チャンネル名が不正
	test_err_476_badchanmask();// エラー: ERR_BADCHANMASK 476 !で始まるチャンネル名が英数5文字 + 1文字以上の名前を満たさない場合
	test_err_471_channelisfull();// エラー: ERR_CHANNELISFULL 471 参加できるユーザー数を超過
	test_err_473_inviteonly();// エラー: ERR_INVITEONLYCHAN 473 招待制で未招待
	test_err_475_badchannelkey();// エラー: ERR_BADCHANNELKEY 475 キー不一致
	std::cout << "JOIN command tests: OK" << std::endl;
	return 0;
}
