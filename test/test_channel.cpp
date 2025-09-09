#include "Database.hpp"
#include <assert.h>
#include <sstream>

int main()
{
	Database db("pass");

	// コンストラクタの初期化確認
	Client *ope0 = new Client();
	ope0->setNickname("0");
	std::string name0 = "channel0";

	Channel channel0(name0, ope0);

	std::map<int, Client> clients = channel0.getClients();
	assert(channel0.getName() == name0);
	assert(clients.size() == 1);
	assert(clients.begin()->second.getNickname() == ope0->getNickname());
	assert(channel0.getChannelOperator() == ope0);


	// チャンネルを新規作成する

	Client *ope = new Client();
	ope->setNickname("1");
	std::string name = "channel1";
	Channel channel1("channel1", ope);

	std::stringstream ss;
	for (int i = 0;i < 3; i++)
	{
		ss << i;
		Client *cre = new Client();
		cre->initializeClient(i + 3);
		cre->setNickname("nusu" + ss.str());
		channel1.addClient(*cre);
		ss.clear();
	}

	channel1.setTopic("aaaa");
	db.addChannel(channel1);
	const Channel *result = db.getChannel(name);

	assert(result->getName() == name);
	assert(result->getTopic() == "aaaa");
	assert(result->getChannelOperator()->getNickname() == "1");

	name = "channel2";
	Channel channel2(name, ope);
	for (int i = 0;i < 3; i++)
	{
		ss << i;
		Client *cre = new Client();
		cre->setNickname("nusu" + ss.str());
		channel2.addClient(*cre);
		ss.clear();
	}
	channel2.setTopic("bbbbb");
	db.addChannel(channel2);
	result = db.getChannel(name);
	assert(result->getName() == name);
	assert(result->getTopic() == "bbbbb");
	assert(result->getChannelOperator()->getNickname() == "1");

	// チャンネルを削除する
	db.removeChannel(name);
	result = db.getChannel(name);
	assert(result == NULL);

	// ユーザーを追加する
	name = "channel1";
	Client client_add;
	client_add.setNickname("add!");
	db.getChannel(name)->addClient(client_add);
	std::map<int, Client> test = db.getChannel(name)->getClients();
	assert(test.size() == 4);
	std::map<int, Client>::iterator it = test.begin();
	while (it != test.end())
	{
		assert(test[client_add.getFd()].getNickname() == "add!");
		it++;
	}

	// ユーザーの削除 Client*
	db.getChannel(name)->removeClient(&client_add); // ここでコケてる
	test = db.getChannel(name)->getClients();
	assert(test.size() == 3);
	it = test.begin();
	while (it != test.end())
	{
		assert(it->first != client_add.getFd());
		it++;
	}

	// ユーザーの削除　fd
	db.getChannel(name)->removeClient(3);
	test = db.getChannel(name)->getClients();
	assert(test.size() == 2);
	it = test.begin();
	while (it != test.end())
	{
		assert(it->first != 3);
		it++;
	}

	return (0);
}
