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

	// [Test] チャンネル名の正規化（小文字化） コンストラクタとgetChannel()
	{
		Client op;
		op.setFd(20);
		Channel ch("ChannelUPPER", op.getFd());
		assert(ch.getName() == "channelupper");

		// Database へ登録して小文字キーで取得できることを確認
		db.addChannel(ch);
		std::string name = "ChannelUpper";
		const Channel* got = db.getChannel(name);
		assert(got != NULL);
		assert(got->getName() == "channelupper");
	}

	// [Test] removeChannel()の大小無視削除
	{
		Client op2;
		op2.setFd(21);
		Channel ch2("MiXeDName", op2.getFd());
		db.addChannel(ch2);

		// 異なる大文字小文字で削除して、取得できないことを確認
		std::string removeName = "MIXEDNAME";
		db.removeChannel(removeName);
		std::string name = "mixedname";
		assert(db.getChannel(name) == NULL);
	}

	// [Test] 招待制 (+i/-i) と招待リスト操作、clearInvitesでフラグも降りる
	{
		Client op;
		op.setFd(30);
		Channel ch("inviteOnly", op.getFd());
		assert(ch.getInviteOnly() == false);

		ch.setInviteOnly(true);//+i
		assert(ch.getInviteOnly() == true);

		int userFd = 31;
		assert(ch.isInvited(userFd) == false);
		ch.addInvite(userFd);
		assert(ch.isInvited(userFd) == true);
		ch.removeInvite(userFd);
		assert(ch.isInvited(userFd) == false);
		ch.addInvite(userFd);

		ch.clearInvites();//-i
		assert(ch.isInvited(userFd) == false);
		assert(ch.getInviteOnly() == false);
	}

	// [Test] トピック保護 (+t/-t)
	{
		Client op;
		op.setFd(32);
		Channel ch("topicRestricted", op.getFd());
		assert(ch.getTopicRestricted() == true);//tは、デフォルトでtrue

		ch.setTopicRestricted(false);//-t
		assert(ch.getTopicRestricted() == false);
		ch.setTopicRestricted(true);//+t
		assert(ch.getTopicRestricted() == true);
	}

	// [Test] キー (+k/-k)
	{
		Client op;
		op.setFd(33);
		Channel ch("withKey", op.getFd());
		assert(ch.getHasKey() == false);

		ch.setKey("secret");//+k
		assert(ch.getHasKey() == true);
		assert(ch.getKey() == std::string("secret"));

		ch.clearKey();//-k
		assert(ch.getHasKey() == false);
		assert(ch.getKey() == std::string(""));
	}

	// [Test] リミット (+l/-l)
	{
		Client op;
		op.setFd(34);
		Channel ch("withLimit", op.getFd());
		assert(ch.getHasLimit() == false);

		ch.setLimit("42");//+l
		assert(ch.getHasLimit() == true);
		assert(ch.getLimit() == std::string("42"));
		ch.clearLimit();//-l
		assert(ch.getHasLimit() == false);
		assert(ch.getLimit() == std::string(""));
	}

	return (0);
}
