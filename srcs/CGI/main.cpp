
#include "CgiHandler.hpp"

int main()
{
	CgiHandler cgi;
	std::string ret;

	ret = cgi.execScript("/home/julien/Cursus42/webserv/cgi-bin/displayEnv");
	std::cout << ret << std::endl;
	return 0;
}