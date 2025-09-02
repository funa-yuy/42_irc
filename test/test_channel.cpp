#include "Database.hpp"
#include <assert.h>
#include <sstream>

int main()
{
	Database db("pass");

	// チャンネルを新規作成する

	Channel channel1;
	Client *ope = new Client();
	ope->setNickname("1");

	std::stringstream ss;
	for (int i = 0;i < 3; i++)
	{
		ss << i;
		Client *cre = new Client();
		cre->setNickname("nusu" + ss.str());
		channel1.addClient(*cre);
		ss.clear();
	}

	std::string name = "channel1";
	channel1.setName(name);
	channel1.setTopic("aaaa");
	channel1.setChannelOperator(ope);
	db.addChannel(channel1);
	const Channel *result = db.getChannel(name);

	assert(result->getName() == name);
	assert(result->getTopic() == "aaaa");
	assert(result->getChannelOperator()->getNickname() == "1");

	Channel channel2;
	name = "channel2";
	channel2.setName(name);
	for (int i = 0;i < 3; i++)
	{
		ss << i;
		Client *cre = new Client();
		cre->setNickname("nusu" + ss.str());
		channel2.addClient(*cre);
		ss.clear();
	}
	channel2.setTopic("bbbbb");
	channel2.setChannelOperator(ope);
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
	std::vector<Client *> test = db.getChannel(name)->getClients();
	assert(test.size() == 4);
	assert(test[3]->getNickname() == "add!");

	// ユーザーの削除 Client*
	db.getChannel(name)->removeClient(&client_add);
	test = db.getChannel(name)->getClients();
	assert(test.size() == 3);
	for (int i = 0; i < (int)test.size();i++)
	{
		assert(test[i]->getNickname() != "add!");
	}

	// ユーザーの削除 string
	std::string nickname = "nusu0";
	db.getChannel(name)->removeClient(nickname);
	test = db.getChannel(name)->getClients();
	assert(test.size() == 2);
	for (int i = 0; i < (int)test.size();i++)
	{
		assert(test[i]->getNickname() != nickname);
	}
	return (0);
}
