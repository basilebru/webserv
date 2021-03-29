#include "Server.hpp"
#include <csignal>

int main(void)
{
	Server *server =  new Server();

	// if (signal(SIGINT, SIG_IGN) == SIG_ERR)
	// 	return (1);


	server->launch();
	delete server;

	return 0;
}