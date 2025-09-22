#include <cassert>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>

#include "Database.hpp"
#include "Command/TopicCommand.hpp"

static t_parsed makeInput(const std::string& cmd, int fd, const std::vector<std::string>& args) {
	t_parsed in;
	in.cmd = cmd;
	in.client_fd = fd;
	in.args = args;
	return in;
}


static void test_success() {
	Database db("password");
	TopicCommand topic;

	// トピック設定 → ブロードキャスト
	{
		int op_fd = 40;
		Client* op = db.addClient(op_fd);
		op->setNickname("op40");

		Channel ch("#v3", op_fd); // OP作成
		db.addChannel(ch);

		int member_fd = 41;
		Client* member = db.addClient(member_fd);
		member->setNickname("mem41");
		Channel* chp = db.getChannel("#v3");
		chp->addClientFd(member_fd);

		std::vector<std::string> args;
		args.push_back("#v3");
		args.push_back("This is a cool channel!!");
		t_parsed in = makeInput("TOPIC", op_fd, args);
		std::vector<t_response> res = topic.execute(in, db);

		// ブロードキャスト1通期待
		assert(res.size() >= 1);
		assert(res[0].reply.find(" TOPIC #v3 :This is a cool channel!!") != std::string::npos);
		const std::vector<int>& fds = res[0].target_fds;
		assert(fds.size() == 2);
		assert(std::find(fds.begin(), fds.end(), op_fd) != fds.end());
		assert(std::find(fds.begin(), fds.end(), member_fd) != fds.end());
	}

	// TOPIC channel のみ（トピック設定済み）→ 332 と 333 を送信者に
	{
		int fd = 31;
		Client* c = db.addClient(fd);
		c->setNickname("u31");

		Channel ch("#ok_topic", fd);
		ch.setTopic("This is topic!!");
		ch.setTopicWho(fd);
		ch.setTopicTime(time(NULL));
		db.addChannel(ch);

		std::vector<std::string> args;
		args.push_back("#ok_topic");
		t_parsed in = makeInput("TOPIC", fd, args);
		std::vector<t_response> res = topic.execute(in, db);

		assert(res.size() == 2);
		if (res[0].reply.find(" 332 ") != std::string::npos) {
			assert(res[0].reply.find(" #ok_topic :This is topic!!") != std::string::npos);
			assert(res[0].target_fds.size() == 1 && res[0].target_fds[0] == fd);
		}
		if (res[1].reply.find(" 333 ") != std::string::npos) {
			assert(res[1].reply.find(" #ok_topic u31 ") != std::string::npos);
			assert(res[1].target_fds.size() == 1 && res[1].target_fds[0] == fd);
		}

	}

	// TOPIC channel のみ（トピック未設定）→ 331 を送信者に
	{
		int fd = 30;
		Client* c = db.addClient(fd);
		c->setNickname("u30");

		Channel ch("#no_topic", fd); // 作成者が参加&OP、トピック未設定
		db.addChannel(ch);

		std::vector<std::string> args;
		args.push_back("#no_topic");
		t_parsed in = makeInput("TOPIC", fd, args);
		std::vector<t_response> res = topic.execute(in, db);

		bool found331 = false;
		for (size_t i = 0; i < res.size(); ++i) {
			if (res[i].reply.find(" 331 ") != std::string::npos) {
				found331 = true;
				assert(res[i].reply.find(" #no_topic ") != std::string::npos);
				assert(res[i].target_fds.size() == 1 && res[i].target_fds[0] == fd);
			}
		}
		assert(found331 == true);
	}

	// 空文字でトピック削除
	{
		int op_fd = 50;
		Client* op = db.addClient(op_fd);
		op->setNickname("op50");

		Channel ch("#clear", op_fd);
		ch.setTopic("will be cleared");
		db.addChannel(ch);

		std::vector<std::string> args;
		args.push_back("#clear");
		args.push_back(""); // 空文字でクリア
		t_parsed in = makeInput("TOPIC", op_fd, args);
		std::vector<t_response> res = topic.execute(in, db);

		// ブロードキャスト1通期待
		assert(res.size() >= 1);
		assert(res[0].reply.find(" TOPIC #clear :") != std::string::npos);
		const std::vector<int>& fds = res[0].target_fds;
		assert(fds.size() == 1);
		assert(std::find(fds.begin(), fds.end(), op_fd) != fds.end());

		Channel* updated = db.getChannel("#clear");
		assert(updated->getTopic().empty());// topicはclearされてる
	}
}

static void test_err_461_needmoreparams() {
	Database db("password");
	TopicCommand topic;

	int fd = 10;
	Client* c = db.addClient(fd);
	c->setNickname("u10");

	std::vector<std::string> args; // 引数なし → 461 期待
	t_parsed in = makeInput("TOPIC", fd, args);
	std::vector<t_response> res = topic.execute(in, db);

	assert(res.size() == 1);
	assert(res[0].is_success == false);
	assert(res[0].should_send == true);
	assert(res[0].reply.find(" 461 ") != std::string::npos);
	assert(res[0].reply.find(" TOPIC :Not enough parameters") != std::string::npos);
	assert(res[0].target_fds.size() == 1 && res[0].target_fds[0] == fd);
}

static void test_err_403_nosuchchannel() {
	Database db("password");
	TopicCommand topic;

	int fd = 11;
	Client* c = db.addClient(fd);
	c->setNickname("u11");

	std::vector<std::string> args;
	args.push_back("#no_such"); // 存在しないチャンネル
	t_parsed in = makeInput("TOPIC", fd, args);
	std::vector<t_response> res = topic.execute(in, db);

	assert(res.size() == 1);
	assert(res[0].is_success == false);
	assert(res[0].should_send == true);
	assert(res[0].reply.find(" 403 ") != std::string::npos);
	assert(res[0].reply.find(" #no_such ") != std::string::npos);
	assert(res[0].target_fds.size() == 1 && res[0].target_fds[0] == fd);
}

static void test_err_442_notonchannel() {
	Database db("password");
	TopicCommand topic;

	int not_member_fd = 12;
	Client* c = db.addClient(not_member_fd);
	c->setNickname("u12");

	// チャンネルは存在するが、クライアントは非メンバー
	int owner_fd = 99;
	db.addClient(owner_fd);
	Channel ch("#ch442", owner_fd);
	db.addChannel(ch);

	std::vector<std::string> args;
	args.push_back("#ch442");
	t_parsed in = makeInput("TOPIC", not_member_fd, args);
	std::vector<t_response> res = topic.execute(in, db);

	assert(res.size() == 1);
	assert(res[0].is_success == false);
	assert(res[0].should_send == true);
	assert(res[0].reply.find(" 442 ") != std::string::npos);
	assert(res[0].reply.find(" #ch442 ") != std::string::npos);
	assert(res[0].target_fds.size() == 1 && res[0].target_fds[0] == not_member_fd);
}

static void test_err_482_chanoprivsneeded() {
	Database db("password");
	TopicCommand topic;

	int op_fd = 20;
	Client* op = db.addClient(op_fd);
	op->setNickname("op20");

	// OPが作成し、topicは +t (restricted)
	Channel ch("#t_restricted", op_fd);
	ch.setTopicRestricted(true);
	db.addChannel(ch);

	// メンバーだがOPではないユーザ
	int member_fd = 21;
	Client* member = db.addClient(member_fd);
	member->setNickname("mem21");
	Channel* chp = db.getChannel("#t_restricted");
	chp->addClientFd(member_fd);

	std::vector<std::string> args;
	args.push_back("#t_restricted");
	args.push_back("new topic");
	t_parsed in = makeInput("TOPIC", member_fd, args);
	std::vector<t_response> res = topic.execute(in, db);

	assert(res.size() == 1);
	assert(res[0].is_success == false);
	assert(res[0].should_send == true);
	assert(res[0].reply.find(" 482 ") != std::string::npos);
	assert(res[0].reply.find(" #t_restricted ") != std::string::npos);
	assert(res[0].target_fds.size() == 1 && res[0].target_fds[0] == member_fd);
}

int main() {
	test_success();
	test_err_461_needmoreparams();
	test_err_403_nosuchchannel();
	test_err_442_notonchannel();
	test_err_482_chanoprivsneeded();

	std::cout << "TOPIC command tests: OK" << std::endl;
	return 0;
}


