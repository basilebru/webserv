#include "CgiHandler.hpp"

int main(int ac, char **av)
{
	if (ac == 2)
	{
		CgiHandler cgi;
		std::string path("../../cgi-bin/" + std::string(av[1]));
		std::string ret;

		ret = cgi.execScript(path);
		std::cout << ret << std::endl;
	}
	return 0;
}