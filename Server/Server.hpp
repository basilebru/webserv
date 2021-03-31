#ifndef SERVER_HPP
# define SERVER_HPP

# include "webserv.hpp"
# include "request_class.hpp"

class Server {

	typedef	Request* Req;

private:
	int					server_socket;
	int					max_socket;
	fd_set				ready_sockets;
	std::map<int, Req>	requests;

	void	setup(int port);
	int		accept_new_connection();
	void	close_socket(std::map<int, Request*>::iterator it);

public:
	static int			server_is_alive;
	Server(void);
	// Server(Server const & copy);
	~Server(void);
	Server& operator=(Server const & rhs);

	int launch(void);
};

std::ostream & operator<<(std::ostream & o, Server const & rhs);

#endif // SERVER_HPP
