# include "../includes/Server.hpp"
# include "../includes/Parser.hpp"

t_parsed Parser::exec(std::string line, int client_fd)
{
	t_parsed parsed;
	std::stringstream stream(line);
	std::vector<std::string> v;

	int pos = line.rfind(":", line.size() - 1);
	if (pos < 0)
		parsed.msg = "";
	else
	{
		parsed.msg = line.substr(pos);
		parsed.msg.erase(0, 1);
		parsed.msg.erase(parsed.msg.size() - 2, 2);
		line.erase(pos);
	}

	std::string temp;
	while (getline(stream, temp, ' '))
	{
		if (temp.size() >= 2 && temp.compare(temp.size() - 2, 2, "\r\n") == 0)
			temp.erase(temp.size() - 2);
		v.push_back(temp);
	}
	parsed.cmd = v[0];
	for (size_t i = 0; i < parsed.cmd.size(); ++i)
		parsed.cmd[i] = toupper(parsed.cmd[i]);
	v.erase(v.begin());
	if (15 < v.size())
		std::cerr << "too many args" << std::endl;
	parsed.args = v;
	parsed.client_fd = client_fd;
	return (parsed);
}
