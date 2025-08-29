#ifndef PRINTLOG_HPP
 #define PRINTLOG_HPP

 #include <string>
 #include <iostream>
 #include <fstream>
 #include <ctime>
 #include <sstream>

class PrintLog
{
	public:
		PrintLog();
		~PrintLog();
		void print_debug(std::string msg);
	private:
		std::ofstream strm;
};

#endif
