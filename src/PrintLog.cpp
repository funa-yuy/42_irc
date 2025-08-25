#include "PrintLog.hpp"

PrintLog::PrintLog()
{
	strm.open("ircserv.log", std::ios::app);
	if (!strm.is_open())
		std::cerr << "Failed to open log file" << std::endl;
}

PrintLog::~PrintLog()
{
	strm.close();
}

void PrintLog::print_debug(std::string msg)
{
	std::cerr << "[DEBUG] " << msg << std::endl;

	std::stringstream ss;
	std::string time;
	std::time_t now = std::time(NULL);
	ss << std::ctime(&now);
	time = ss.str().erase(ss.str().size()-1);
	strm << time << " [DEBUG] :" << msg << std::endl;
	strm.flush();
}
