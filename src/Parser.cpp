# include "../includes/Server.hpp"
# include "../includes/Parser.hpp"

t_parsed Parser::exec(std::string line, int client_fd)
{
	t_parsed parsed;
	std::vector<std::string> v;
	std::string last_arg;

	if (line.empty())
		return parsed;
	int pos = -1;
	for (int i = 0; i < (int)line.size();i++)
	{
		if (line[i] == ' ' && i != (int)line.size() -1 && line[i + 1] == ':')
		{
			pos = i + 1;
			break ;
		}
	}
	if (pos < 0)
		last_arg = "";
	else
	{
		last_arg = line.substr(pos);
		last_arg.erase(0, 1);
		last_arg.erase(last_arg.size() - 2, 2);
		line.erase(pos);
	}

	std::stringstream stream(line);
	std::string temp;
	std::string prev;
	int  i = 0;
	while (getline(stream, temp, ' '))
	{
		if (temp.size() >= 2 && temp.compare(temp.size() - 2, 2, "\r\n") == 0)
			temp.erase(temp.size() - 2);
		if ((!prev.empty() || i == 0) && !temp.empty())
		{
			v.push_back(temp);
			prev = temp;
		}
		i++;
	}
	if (!last_arg.empty())
		v.push_back(last_arg);

	parsed.cmd = v[0];
	for (size_t i = 0; i < parsed.cmd.size(); ++i)
		parsed.cmd[i] = toupper(parsed.cmd[i]);
	v.erase(v.begin());
	if (MAX_MSG_ARG < v.size())
		print_debug("too many args");
	parsed.args = v;
	parsed.client_fd = client_fd;
	return (parsed);
}

void print_debug(std::string msg)
{
	std::cerr << "[DEBUG] " << msg << std::endl;
}
