# include "../includes/Server.hpp"
# include "../includes/Parser.hpp"

t_parsed Parser::exec(std::string line, int client_fd)
{
	t_parsed parsed;
	std::vector<std::string> v;

	std::string last_arg;
	int pos = line.rfind(":", line.size() - 1);
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
	while (getline(stream, temp, ' '))
	{
		if (temp.size() >= 2 && temp.compare(temp.size() - 2, 2, "\r\n") == 0)
			temp.erase(temp.size() - 2);
		v.push_back(temp);
	}
	if (!last_arg.empty())
		v.push_back(last_arg);

	parsed.cmd = v[0];
	for (size_t i = 0; i < parsed.cmd.size(); ++i)
		parsed.cmd[i] = toupper(parsed.cmd[i]);
	v.erase(v.begin());
	if (15 < v.size())
		print_debug("too many args");
	parsed.args = v;
	parsed.client_fd = client_fd;
	return (parsed);
}

void print_debug(std::string msg)
{
	std::cerr << "[DEBUG] " << msg << std::endl;
}
