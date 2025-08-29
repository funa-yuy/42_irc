# include "../includes/Server.hpp"
# include "../includes/Parser.hpp"

t_parsed Parser::exec(std::string line, int client_fd)
{
	t_parsed parsed;
	parsed.client_fd = client_fd;

	while (!line.empty() && (line[line.size() - 1] == '\n' || line[line.size() - 1] == '\r'))
		line.erase(line.size() - 1);

	if (line.find_first_not_of(" \t") == std::string::npos)
	{
		parsed.cmd = "";
		return (parsed);
	}

	std::string	trailing;
	size_t	sep = line.find(" :");
	if (sep != std::string::npos)
	{
		trailing = line.substr(sep + 2);
		line.erase(sep);
	}

	std::stringstream ss(line);
	std::vector<std::string> tokens;
	std::string	token;
	while (ss >> token)
		tokens.push_back(token);

	if (tokens.empty())
	{
		parsed.cmd = "";
		return (parsed);
	}

	parsed.cmd = tokens[0];
	for (size_t i = 0; i < parsed.cmd.size(); ++i)
		parsed.cmd[i] = std::toupper(parsed.cmd[i]);	
	tokens.erase(tokens.begin());

	if (!trailing.empty())
		tokens.push_back(trailing);

	if (tokens.size() > 15)
		std::cerr << "too many args" << std::endl;

	parsed.args = tokens;

	return (parsed);
}
