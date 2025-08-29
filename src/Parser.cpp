# include "Parser.hpp"

t_parsed Parser::exec(std::string line, int client_fd)
{
	t_parsed parsed;
	parsed.client_fd = client_fd;

	trimCRLF(line);
	if (line.find_first_not_of(" \t") == std::string::npos)
	{
		parsed.cmd = "";
		return (parsed);
	}

	std::string	trailing;
	extractTrailing(line, trailing);

	std::vector<std::string>	tokens;
	tokenize(line, tokens);
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

void	Parser::trimCRLF(std::string & s)
{
	while (!s.empty())
	{
		char	c = s[s.size() - 1];
		if (c == '\r' || c == '\n')
			s.erase(s.size() - 1, 1);
		else
			break ;
	}

	return ;
}

void	Parser::extractTrailing(std::string & s, std::string & trailing)
{
	size_t	sep = s.find(" :");
	if (sep != std::string::npos)
	{
		trailing = s.substr(sep + 2);
		s.erase(sep);
	}

	return ;
}

void	Parser::tokenize(std::string & s, std::vector<std::string> & tokens)
{
	std::stringstream ss(s);
	std::string	token;

	while (ss >> token)
		tokens.push_back(token);

	return ;
}
