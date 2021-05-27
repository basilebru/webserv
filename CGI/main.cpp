
#include "CgiHandler.hpp"

int main()
{
	CgiHandler cgi;
	std::string ret;

	ret = cgi.execScript("../cgi-bin/displayEnv");
	std::cout << ret << std::endl;
	return 0;
}