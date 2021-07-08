#include "Server.hpp"

/*Server::Server(void)
{
	return ;
}
*/
Server::Server(HttpBlock const& config, std::vector<ServerBlock> const& srvs)
: baseConfig(config), servers(srvs)
{
	return ;
}

/*Server::Server(Server const & copy)
{
	return ;
}
*/
Server::~Server(void)
{
	// Clean up all of the sockets that are open 
	for (std::map<int, Request*>::const_iterator it = this->client_sockets.begin(); it != this->client_sockets.end();)
	{
		// if (FD_ISSET(it->first, &this->ready_read_sockets))
		this->close_socket(it++->first);
		// else
		// {
		// 	std::cerr << "fd " << it->first << "is still in use." << std::endl;
		// 	it++;
		// }
	}
	for (sockMap::const_iterator it = this->server_sockets.begin(); it != this->server_sockets.end(); ++it)
	{
		if (close(it->first) < 0)
		{
			std::cerr << "Failed to close. errno:" << errno << std::endl;
			exit(EXIT_FAILURE);
		}
		std::cerr << WHITE << "server_socket closed (fd " << it->first << ")."
		<< NOCOLOR <<std::endl;
	}

	return ;
}

int	Server::server_is_alive = 1;


int Server::launch(void)
{
	if (this->setup_sockets() < 0)
		return (FAILURE);
	this->init_select_fd_set();

	//Optional: sets the timeout for select()
	struct timeval timeout;

	while (Server::server_is_alive)
	{
		//Optional: sets the timeout for select()
		// Il s'agit du temps le plus long que select() pourrait attendre avant de rendre la main, même si rien d'intéressant n'est arrivé.
		// Si cette valeur est positionnée à NULL, alors, select() bloque indéfiniment dans l'attente qu'un descripteur de fichier devienne prêt
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;

		if (this->select() == FAILURE)
			return FAILURE;

		// On parcourt nos fd (client sockets et server sockets): pour chaque fd on teste s'il est présent dans this->ready_sockets = disponible en lecture
		this->loop_server_socket();
		this->loop_client_socket();
	}
	return SUCCESS;
}

int	Server::select()
{
	// select is destructive so we need a socket copy
	this->ready_read_sockets = this->current_sockets;
	this->ready_write_sockets = this->current_sockets;
	int sret = ::select(this->max_socket + 1, &this->ready_read_sockets, &this->ready_write_sockets, NULL, NULL);
	// suite à l'appel à select, this->ready_sockets ne contient plus que les fd disponibles en lecture
	// (current_sockets contient toujours tous les sockets existants)

	// En cas de succès, select() renvoie le nombre total de descripteurs de fichiers encore présents dans les ensembles de descripteurs de fichier.
	// En cas de timeout échu, alors les fd_set devraient tous être vides -> la valeur renvoyée est zéro
	// En cas d'erreur, renvoie -1	
	if (sret < 0 && errno != EINTR)
	{
		std::cerr << "Select error. errno: " << errno << std::endl;
		return (FAILURE);
	}
	else if (sret == 0)
		std::cerr << sret << ": Select timeout (5 sec)" << std::endl;
	this->rdy_fd = sret;
	return (SUCCESS);
}

int		Server::loop_server_socket()
{
	// On parcourt les server_sockets. S'ils sont disponibles en lecture, c'est qu'il y a une nouvelle demande de connection -> accept connection et creation d'un client socket
	int client_socket;
	for (sockMap::const_iterator it = this->server_sockets.begin(); it != this->server_sockets.end(); ++it)
	{
		if (FD_ISSET(it->first, &this->ready_read_sockets))
		{
			this->rdy_fd--;
			client_socket = this->accept_new_connection(it->first);
			if (client_socket < 0 && errno != EAGAIN) 
			{
				std::cout << "Failed to grab connection. errno: " << errno << std::endl;
				return (FAILURE);
			}
			else if (client_socket > 0)
			{
				std::cout << YELLOW << "New incoming connection (fd " << client_socket << ")" << NOCOLOR << std::endl;
				FD_SET(client_socket, &this->current_sockets); // new connection is added to fd_set (current socket)
				Request *req = new Request(client_socket, it->second, this->servers, this->baseConfig);
				this->client_sockets.insert(std::make_pair(client_socket, req));

				if (client_socket > this->max_socket)
					this->max_socket = client_socket;
			}
		}
	}
	return SUCCESS;
}


// returns CLOSE if connection needs to be closed (error during request or EOF received)
// returns SKIP if request is not ready to be processed (not entirely received)
// returns NEW if request has been processed, buffer is filled -> request can be deleted
int		Server::process_request(std::map<int, Request*>::iterator client_socket_pair_iterator)
{
	Request* request = client_socket_pair_iterator->second;
	int client_socket_fd = client_socket_pair_iterator->first;
	request->parse();
	if (request->connection_end())
	{
        	std::cout << RED << "Client closed connection !" << NOCOLOR << std::endl;
		return CLOSE;
	}
	if (request->get_error_code() || request->request_is_ready())
	{
		int ret;
		std::vector<unsigned char>& response_buffer = this->response_buffers[client_socket_fd];
		Response *response = new Response(*request, response_buffer);
		ret = response->build();
		delete response;
		return ret;
	}
	return SKIP;
}

int		Server::loop_client_socket()
{
	// On parcourt les client_sockets. S'ils sont disponibles en lecture, c'est qu'on peut parser une requete (rq: une meme requete peut etre envoyee en plusieurs fois)
	for (std::map<int, Request*>::iterator it = this->client_sockets.begin(); it != this->client_sockets.end() && this->rdy_fd > 0;)
	{
		int ret(0);
		if (FD_ISSET(it->first, &this->ready_read_sockets)) // read() possible
		{
			// std::cout << GREEN << "Communication with client -> fd " << it->first << NOCOLOR << std::endl;
			ret = this->process_request(it);
			if (ret == NEW)
			{
				// save fd and addr, delete request and create new request
				int fd = it->second->get_fd();
				sockaddr_in addr = it->second->get_addr();
				delete it->second;
				it->second = new Request(fd, addr, this->servers, this->baseConfig);
			}
		}
		if (FD_ISSET(it->first, &this->ready_write_sockets) && this->response_buffers[it->first].size()) // write possible
		{
			ssize_t ret;
			ret = send(it->first, &this->response_buffers[it->first][0], this->response_buffers[it->first].size(), MSG_DONTWAIT);
			if (ret == -1)
			{
				std::cerr << strerror(errno) << std::endl;
				this->close_socket(it++->first);
				continue;
			}
			// std::cout << "AFTER SEND: " << ret << std::endl;
			this->response_buffers[it->first].erase(this->response_buffers[it->first].begin(), this->response_buffers[it->first].begin() + ret);
		}
		if (ret == CLOSE && this->response_buffers[it->first].empty())
		{
			this->close_socket(it++->first); // use post incrementation in order to "copy" next element before deleting current element
			continue;
		}
		it++;
	}
	return SUCCESS;
}

bool my_comp(ServerBlock serv1, ServerBlock serv2)
{
	if (serv1.getListenIP() < serv2.getListenIP())
		return true;
	return false;
}

int Server::setup_sockets(void)
{
	// Create this->server_sockets; for each server_socket, bind and listen
	
	// displayVec(this->servers, '\n');

	std::vector<ServerBlock>::const_iterator it = this->servers.begin();
	while (it != this->servers.end())
	{

		// 1. Create a socket (IPv4, TCP)
		int newSocket;
		memset((char *)&newSocket, 0, sizeof(newSocket)); 
		newSocket = socket(AF_INET, SOCK_STREAM, 0); // Using the flag SOCK_NONBLOCK saves extra calls to fcntl(2) to achieve the same result.
		if (newSocket == -1)
		{
			std::cout << "Failed to create socket. errno: " << errno << std::endl;
			return (FAILURE);
		}
		// 2. Set the socket as NON BLOCKING (for MAC, no need in Linux ?)
		if (fcntl(newSocket, F_SETFL, O_NONBLOCK) < 0)
			std::cout << "Fcntl failed. errno: " << errno << std::endl;

		// 3. Set socket options
		// SO_REUSEADDR allows your server to bind to an address which is in a TIME_WAIT state 
		// --> In order to reuse the socket quickly after stopping and restarting the server
		// SO_REUSEPORT allows multiple processes to bind to the same address (provided all of them use the SO_REUSEPORT option)
		// --> So we can have different server blocks with identical listen directives
		int optval = 1; // The optval sets to 1 (or > 0) enables teh OPTION, sets to 0 disable it
		if (setsockopt(newSocket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) < 0)
			std::cout << "Error: setsockopt(SO_REUSEPORT) failed. errno: " << errno << std::endl;
		if (setsockopt(newSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
			std::cout << "Error: setsockopt(SO_REUSEADDR) failed. errno: " << errno << std::endl;

		// 4. bind to addresses and port found in Config
		sockaddr_in sockaddr;
		memset((char *)&sockaddr, 0, sizeof(sockaddr)); 
		sockaddr.sin_family = AF_INET;
		sockaddr.sin_addr.s_addr = it->getListenIP(); // Adresse dans l'ordre d'octets réseau
		sockaddr.sin_port = it->getListenPort(); // htons is necessary to convert a number to
		std::cerr << "IP: " << ipToString(sockaddr.sin_addr.s_addr) << std::endl;
		std::cerr << "PORT: " << ntohs(it->getListenPort()) << std::endl;
		if (bind(newSocket, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
		{
			close(newSocket);
			std::cout << "Failed to bind to port " << ntohs(sockaddr.sin_port) << " on address " << ::ipToString(sockaddr.sin_addr.s_addr)
			 << ". errno: " << errno << std::endl;
			return (FAILURE);
		}
		
		// 5. Start listening. Hold at most 100 connections in the queue
		if (listen(newSocket, 100) < 0) // On met combien au backlog ?
		{
			std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
			close(newSocket);
			return (FAILURE);
		}
		this->server_sockets.insert(std::pair<int, sockaddr_in>(newSocket, sockaddr));

		++it;
	}

	// if (this->server_sockets.size() == 0)
	// 	return (FAILURE);
	
	return (SUCCESS);
}

int Server::accept_new_connection(int server_socket)
{
	int	client_socket;

	client_socket = accept(server_socket, NULL, NULL);
	// if (client_socket < 0)
	// 	std::cout << "Failed to grab connection. errno: " << errno << std::endl;
	// else
	// {
	// 	if (fcntl(client_socket, F_SETFL, O_NONBLOCK) < 0)
	// 	std::cout << "Fcntl failed. errno: " << errno << std::endl;
	// }
	return (client_socket);
}

void Server::close_socket(int fd)
{
	FD_CLR(fd, &this->current_sockets);
	if (fd == this->max_socket)
		this->max_socket--;
	if (close(fd) < 0)
	{
		std::cerr << "Failed to close. errno:" << errno << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << YELLOW << "Client " << fd << " disconnected." << NOCOLOR << std::endl;
	// delete the Request 
	delete this->client_sockets[fd];
	// Erase the map element containing the former request
	this->client_sockets.erase(fd);
	this->response_buffers.clear();
}

void	Server::init_select_fd_set()
{
	// Prepare for select()
	// 1. Initialize the current socket set
	FD_ZERO(&this->current_sockets);
	// 2. Add all servers socket in socket SET
	for (sockMap::const_iterator it = this->server_sockets.begin(); it != this->server_sockets.end(); ++it)
		FD_SET(it->first, &this->current_sockets);
	// 3. Set max socket (select needs the highest-numbered file descriptor in any of the three sets, plus 1)
	this->max_socket = this->server_sockets.rbegin()->first;
	std::cerr << "MAX SOCKET: " << this->max_socket << std::endl;
}

