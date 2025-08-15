#include <iostream> //cout

#include "PassCommand.hpp"
#include "NickCommand.hpp"
#include "UserCommand.hpp"

// ```bash
// make re MAIN=test_cmd.cpp
// valgrind ./test_irc
// ```

t_response	handleInput(const t_parsed& input, Database& db) {
	Command* cmd = NULL;

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
		return (res);
	}
	t_response	res = cmd->execute(input, db);
	delete	cmd;
	return (res);
}

int	main() {
	int	n = 4;
	t_parsed	inputs[4];
	Database	db("password");

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
		t_response res = handleInput(inputs[i], db);
		std::cout << "コマンド: " << inputs[i].cmd << " → 実行結果: " << res.reply << std::endl;
	}

	return (0);
}
