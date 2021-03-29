#include "Server.hpp"

int main(void)
{
	Server *server =  new Server();

	server->launch();
	delete server;

	return 0;
}