#include "Database.hpp"
#include <assert.h>
#include <sstream>

int main()
{
	Database db("pass");

	// [Test] コンストラクタの初期化確認
	{
		Client ope0;
		ope0.setFd(4);
		std::string name0 = "channel0";

		Channel channel0(name0, ope0.getFd());

		const std::set<int>& clients = channel0.getClientFds();
		assert(channel0.getName() == name0);
		assert(clients.size() == 1);
		assert(clients.find(ope0.getFd()) != clients.end());
		const std::set<int>& ops = channel0.getChannelOperatorFds();
		assert(ops.size() == 1 && ops.find(ope0.getFd()) != ops.end());
	}

	// [Test] チャンネル作成と参加者追加/トピック/登録確認
	std::string name;
	Client ope;
	const Channel *result;
	{
		name = "channel1";
		ope.setFd(5);
		Channel channel1("channel1", ope.getFd());

		for (int i = 0; i < 3; i++)
		{
			Client cre;
			cre.setFd(i + 6);
			channel1.addClientFd(cre.getFd());
		}

		channel1.setTopic("aaaa");
		db.addChannel(channel1);
		result = db.getChannel(name);
		assert(result->getName() == name);
		assert(result->getTopic() == "aaaa");
		const std::set<int>& ops = result->getChannelOperatorFds();
		assert(ops.size() == 1 && ops.find(ope.getFd()) != ops.end());
	}

	// [Test] 別チャンネルの作成/登録確認/削除
	{
		name = "channel2";
		Channel channel2(name, ope.getFd());
		for (int i = 0; i < 3; i++)
		{
			Client cre;
			cre.setFd(i + 9);
			channel2.addClientFd(cre.getFd());
		}
		channel2.setTopic("bbbbb");
		db.addChannel(channel2);
		result = db.getChannel(name);
		assert(result->getName() == name);
		assert(result->getTopic() == "bbbbb");
		const std::set<int>& ops = result->getChannelOperatorFds();
		assert(ops.size() == 1 && ops.find(ope.getFd()) != ops.end());

		// チャンネルを削除する
		db.removeChannel(name);
		result = db.getChannel(name);
		assert(result == NULL);
	}

	// [Test] 参加者の追加と削除、存在しない削除リクエスト
	{
		name = "channel1"; // すでに4人の参加者がいるチャンネル
		Client client_add;
		client_add.setFd(10);
		db.getChannel(name)->addClientFd(client_add.getFd());
		std::set<int> test = db.getChannel(name)->getClientFds();
		assert(test.size() == 5);
		assert(test.find(client_add.getFd()) != test.end());

		// ユーザーの削除 fd
		db.getChannel(name)->removeClientFd(client_add.getFd());
		test = db.getChannel(name)->getClientFds();
		assert(test.size() == 4);
		assert(test.find(client_add.getFd()) == test.end());

		// 存在しないユーザーの削除 fd
		assert(test.find(3) == test.end());
		db.getChannel(name)->removeClientFd(3);
		test = db.getChannel(name)->getClientFds();
		assert(test.size() == 4);
		assert(test.find(3) == test.end());
	}

	return (0);
}
