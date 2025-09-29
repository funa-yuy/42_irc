#include "Server.hpp"

int	main(int argc, char *argv[])
{
	if (argc != 3)
	{
		std::cerr << "Error: usage: ./ircserv [port] [password]" << std::endl;
		return (1);
	}

	errno = 0;
	char	*end = 0;
	long	port = std::strtol(argv[1], &end, 10);
	if (errno == ERANGE || end == argv[1] || *end != '\0')
	{
		std::cerr << "Error: invalid port number. Use 1-65535" << std::endl;
		return (1);
	}
	if (port < 1 || port > 65535)
	{
		std::cerr << "Error: port out of range. Use 1-65535" << std::endl;
		return (1);
	}

	std::string	password = argv[2];
	if (password.empty())
	{
		std::cerr << "Error: password must not be empty" << std::endl;
		return (1);
	}
	if (password.find_first_of(" \t\r\n\v\f") != std::string::npos)
    {
        std::cerr << "Error: password must not contain whitespace" << std::endl;
        return (1);
    }

	Server	server(static_cast<int>(port), password);
	server.run();
	return (0);
}
