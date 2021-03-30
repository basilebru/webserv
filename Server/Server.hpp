#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <iostream>
# include <sys/socket.h> // For socket functions
# include <netinet/in.h> // For sockaddr_in
# include <cstdlib> // For exit() and EXIT_FAILURE
# include <unistd.h> // For read
# include <fstream> // ifstream
# include <sstream> // sstream
# include <fcntl.h> // For fcntl --> non_blocking fd
# include <map>

# include "request_class.hpp"

class Server {

	typedef	Request* Req;

private:
	int max_socket;
	fd_set ready_sockets;
	std::map<int, Req> requests;

	int setup(int port);
	int accept_new_connection(int server_socket);
	void close_socket(std::map<int, Request*>::iterator it);

public:
	Server(void);
	// Server(Server const & copy);
	~Server(void);
	Server& operator=(Server const & rhs);

	int launch(void);
};

std::ostream & operator<<(std::ostream & o, Server const & rhs);

#endif // SERVER_HPP
