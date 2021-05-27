#ifndef SERVER_HPP
# define SERVER_HPP

# include "webserv.hpp"
# include "request_class.hpp"
# include "Response.hpp"
# include "HttpBlock.hpp"
# include "ServerBlock.hpp"
# include "utils.hpp"

class Server {

	typedef	std::vector<ServerBlock>	servVec;

private:
	fd_set						ready_read_sockets; // "ready" sockets (after a call to select())
	fd_set						ready_write_sockets; // "ready" sockets (after a call to select())
	fd_set 						current_sockets; // all client and server sockets
	int 						rdy_fd;
	int							max_socket;
	HttpBlock const&			baseConfig;
	servVec const&				servers;
	std::map<int, sockaddr_in>	server_sockets; // stores address (IP + port) of each server socket
	std::map<int, Request*>		client_sockets;
	std::map<int, std::string>	response_buffers;
	
	int		setup_sockets(void);
	int		accept_new_connection(int socket);
	void	close_socket(int fd);
	void	init_select_fd_set();
	int		select();
	int		loop_server_socket();
	int		loop_client_socket();
			Server(void);

public:
	typedef	std::map<int, sockaddr_in>	sockMap;
	
	static int			server_is_alive;
	Server(HttpBlock const& config, servVec const& srvs);
	// Server(Server const & copy);
	~Server(void);
	Server& operator=(Server const& rhs);

	int launch(void);
};

std::ostream & operator<<(std::ostream & o, Server const & rhs);

#endif // SERVER_HPP
