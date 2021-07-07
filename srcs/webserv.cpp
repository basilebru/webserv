#include "Server.hpp"
#include "ConfParser.hpp"
#include "HttpBlock.hpp"
#include <csignal> // To handle CTRL-C (and others signals ?)

void signal_handler(int signum)
{
	if (signum == SIGINT)
	{
		Server::server_is_alive = 0;
	}
	if (signum == SIGPIPE)
	{
		std::cout << "SP caught" << std::endl;
	}
}

void displayConfig(HttpBlock const& baseConfig, std::vector<ServerBlock> const& servers)
{
	std::cout << std::endl << "WEBSERV CONFIGURATION: " << std::endl;
	std::cout << "Nbr of servers: " << servers.size() << std::endl << std::endl;
	
	std::cout << baseConfig;
	displayVec(servers, "\n");
}

int main(int ac, char **av)
{
	if (signal(SIGINT, signal_handler) == SIG_ERR)
		return (1);
	if (signal(SIGPIPE, signal_handler) == SIG_ERR)
		return (1);

	if (ac >= 1 && ac < 3)
	{
		ConfParser parser;

		try {
			parser.readConfFile(DEFAULT_CONF_FILE); // Default config file containing http directive
			
			if (ac == 1)
				parser.readConfFile(DEFAULT_SERVER); // Default server
			if (ac == 2)
				parser.readConfFile(av[1]); // Config file containing server blocks

			HttpBlock baseConfig = parser.getHttpBlock();
			std::vector<ServerBlock> servers = parser.getServers();

			// displayConfig(baseConfig, servers);
		
			/*Create and aunch server*/
			Server *server =  new Server(baseConfig, servers);
			server->launch();
			delete server;
		}
		catch(const std::exception& e) {
			std::cerr << e.what() << '\n';
		}
	}
	else
		write(2, "Execute with zero (default) or one config file in argument\n", 59);
	return (0);
}
