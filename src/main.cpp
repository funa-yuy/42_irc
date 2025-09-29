#include "Server.hpp"

int	main(int argc, char *argv[])
{
	if (argc != 3)
	{
		std::cerr << "Error: usage: ./ircserv [port] [password]" << std::endl;
		return (1);
	}

	int			port = atoi(argv[1]);
	std::string	password = argv[2];

	Server	server(port, password);
	server.run();
	return (0);
}
