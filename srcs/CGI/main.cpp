#include "CgiHandler.hpp"
#include "request_class.hpp"
#include "HttpBlock.hpp"
#include "ServerBlock.hpp"

int main(int ac, char **av)
{
	if (ac == 2)
	{
		sockaddr_in addr;
		std::vector<ServerBlock> srvs;
		HttpBlock base_config;

		Request req(3,  addr, srvs, base_config);
		CgiHandler cgi(req);

		std::string path("../../cgi-bin/" + std::string(av[1]));
		std::vector<unsigned char> v;

		v = cgi.execScript(path);
		displayVec(v);
	}
	return 0;
}