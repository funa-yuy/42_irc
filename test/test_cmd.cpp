#include <iostream> //cout

#include "Command/PassCommand.hpp"
#include "Command/NickCommand.hpp"
#include "Command/UserCommand.hpp"

// ```bash
// make re MAIN=test_cmd.cpp
// valgrind ./test_irc
// ```

std::vector<t_response>	handleInput(const t_parsed& input, Database& db) {
	Command* cmd = NULL;
	std::vector<t_response> response_list;

	if (input.cmd == "PASS") {
		cmd = new PassCommand();
	}
	else if (input.cmd == "NICK") {
		cmd = new NickCommand();
	}
	else if (input.cmd == "USER") {
		cmd = new UserCommand();
	}
	else {
		t_response res;
		res.reply = "Unknown command\n";
		res.target_fds.push_back(0);
		response_list.push_back(res);
		return (response_list);
	}
	response_list = cmd->execute(input, db);
	delete	cmd;
	return (response_list);
}

int	main() {
	int	n = 4;
	t_parsed	inputs[4];
	Database	db("password");
	db.addClient(0);

	inputs[0].cmd = "PASS";
	inputs[0].client_fd = 0;
	inputs[0].args.push_back("test");

	inputs[1].cmd = "NICK";
	inputs[1].client_fd = 0;
	inputs[1].args.push_back("test");

	inputs[2].cmd = "USER";
	inputs[2].client_fd = 0;
	inputs[2].args.push_back("test");

	inputs[3].cmd = "NOEXSIT";
	inputs[3].client_fd = 0;
	inputs[3].args.push_back("test");

	for (int i = 0; i < n; ++i)
	{
<<<<<<< HEAD
		std::vector<t_response> response_list = handleInput(inputs[i], db);
		for (size_t j = 0; j < response_list.size(); ++j)
		{
			const t_response & res = response_list[j];
			std::cout << "コマンド: " << inputs[i].cmd << " → 実行結果: " << res.reply << std::endl;
		}
=======
		t_response res = handleInput(inputs[i], db);
		// std::cout << "コマンド: " << inputs[i].cmd << " → 実行結果: " << res.reply << std::endl;
>>>>>>> ad6c508ebf53c02e959a031799de306e0fe2825b
	}

	return (0);
}
