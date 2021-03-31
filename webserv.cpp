#include "Server.hpp"
#include <csignal> // To handle CTRL-C (and others signals ?)

void signal_handler(int signum)
{
	if (signum == SIGINT)
	{
		Server::server_is_alive = 0;
		std::cerr << std::endl;
	}
}

int main(void)
{
	Server *server =  new Server();

	if (signal(SIGINT, signal_handler) == SIG_ERR)
		return (1);

	server->launch();
	delete server;

	return (0);
}
