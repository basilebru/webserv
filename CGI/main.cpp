
#include "CgiHandler.hpp"

int main()
{
	CgiHandler cgi;

	cgi.execScript("../cgi-bin/displayEnv");
	return 0;
}