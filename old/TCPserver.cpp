#include "webserv.hpp"
#include <csignal>
#include <fcntl.h>
#include <vector>


int setup(int port)
{
	int server_socket; 
	sockaddr_in sockaddr;

	// Create a socket (IPv4, TCP)
	// Using the flag SOCK_NONBLOCK saves extra calls to fcntl(2) to achieve the same result.
	server_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0); 
	if (server_socket == -1) {
		std::cout << "Failed to create socket. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}

	// Set the socket as NON BLOCKING (for MAC, no need in Linux ?)
	if (fcntl(server_socket, F_SETFL, O_NONBLOCK) < 0)
		std::cout << "Fcntl failed. errno: " << errno << std::endl;

	// In order to reuse the socket quickly after stopping and restarting the server
	// Don't take TIME_WAIT into consideration
	int optval = 1; // The optval sets to 1 (or > 0) enables teh OPTION, sets to 0 disable it
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
		std::cout << "Error: setsockopt(SO_REUSEADDR) failed. errno: " << errno << std::endl;

	
	// Listen to port 9999 on any address
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons(port); // htons is necessary to convert a number to
																	 // network byte order
	if (bind(server_socket, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
		std::cout << "Failed to bind to port 9999. errno: " << errno << std::endl;
		close(server_socket);
		exit(EXIT_FAILURE);
	}
	// Start listening. Hold at most 10 connections in the queue
	if (listen(server_socket, 10) < 0) {
		std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
		close(server_socket);
		exit(EXIT_FAILURE);
	}


	return server_socket;
}

int accept_new_connection(int server_socket)
{
	int			client_socket;
	sockaddr_in client_sockaddr;
	size_t		addrlen = sizeof(client_sockaddr);

	client_socket = accept(server_socket, (struct sockaddr*)&client_sockaddr, (socklen_t*)&addrlen);
	return client_socket;
}


int main() {


	fd_set current_sockets, ready_sockets;
	int max_socket = 0;
	int rdy_fd = 0;
	int client_socket;

	// Create socket, binf and listen
	int server_socket = setup(9999);


	//Initilaize the current socket set
	FD_ZERO(&current_sockets);
	FD_SET(server_socket, &current_sockets);
	// au début, notre set (current_sockets) ne contient que 1 socket: server_socket que nous avons créé plus haut
	max_socket = server_socket;
	printf("server_socket: %d\n", max_socket);
	// printf("FD_SETSIZE: %d\n", FD_SETSIZE);

	//Optional: sets the timeout for select()
	struct timeval timeout;

    // process requests until an error is found on a request
	std::string request_ok = "Request received :)\n";
	std::string request_ko = "Request error :( \n";
	while (1)
	{
		//Optional: sets the timeout for select()
		// Il s'agit du temps le plus long que select() pourrait attendre avant de rendre la main, même si rien d'intéressant n'est arrivé.
		// Si cette valeur est positionnée à NULL, alors, select() bloque indéfiniment dans l'attente qu'un descripteur de fichier devienne prêt
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;

		// select is destructive so we need a socket copy
		ready_sockets = current_sockets;
		int sret = select(max_socket + 1, &ready_sockets, NULL, NULL, NULL);
		// suite à l'appel à select, ready_sockets ne contient plus que les fd disponibles en lecture
		// (current_sockets contie)nt toujours tous les sockets existants)

		// En cas de succès, select() renvoie le nombre total de descripteurs de fichiers encore présents dans les ensembles de descripteurs de fichier.
		// En cas de timeout échu, alors les fd_set devraient tous être vides -> la valeur renvoyée est zéro
		// En cas d'erreur, renvoie -1	
		if (sret < 0)
		{
			std::cerr << "Select error. errno: " << errno << std::endl;
			exit(EXIT_FAILURE);
		}
		else if (sret == 0)
			std::cerr << sret << ": Select timeout (5 sec)" << std::endl;

		// On parcourt notre fd_set (current socket): pour chaque fd on teste s'il est présent dans ready_sockets = disponible en lecture
		rdy_fd = sret;
		for (int i = 0; i < max_socket + 1 && rdy_fd > 0; ++i)
		{
			if (FD_ISSET(i, &ready_sockets)) 
			{
				if (i == server_socket) // a new connection has arrived -> grab connection from the queue (cf "listen" plus haut)
				{
					rdy_fd -= 1;
					client_socket = accept_new_connection(server_socket);
					if (client_socket < 0) 
					{
						std::cout << "Failed to grab connection. errno: " << errno << std::endl;
						exit(EXIT_FAILURE);
					}
					printf("New incoming connection (fd %d)\n", client_socket);
					FD_SET(client_socket, &current_sockets); // new connection is added to fd_set (current socket)
					if (client_socket > max_socket)
						max_socket = client_socket;
				}
				else // read from existing connection
				{
					printf("Communication with client - fd %d\n", i);
					// std::vector<Request> v;
					Request *req = new Request(i);
					// v.push_back(req);
					// int ret = v[i].parse();
					int ret = req->parse();
					printf("ret = %d\n", ret);

					if (ret == 0)
					{
						if (close(i) < 0)
						{
							std::cerr << "Failed to close. errno:" << errno << std::endl;
							exit(EXIT_FAILURE);
						}
						FD_CLR(i, &current_sockets);

						if (i == max_socket)
							max_socket -= 1;
						printf("fd %d closed.\n", i);
						printf("max_socket %d\n", max_socket);
					}

					// send() errors handling
					if (req->get_error_code())
						send(i, request_ko.c_str(), request_ko.size(), 0);
					else
						send(i, request_ok.c_str(), request_ok.size(), 0);
					if (ret != 0)
						req->print();
					if (req->get_error_code())
					{
						delete req;
						break;
					}
					delete req;
				}
			}
		}
	}
	
	// Clean up all of the sockets that are open 
	for (int i = 0; i <= max_socket; ++i)
	{
		if (FD_ISSET(i, &ready_sockets))
		{
			close(i);
			printf("fd %d closed.\n", i);
		}
	}

	
	// Send a message to the client_socket

    // // Send content of a file
	// 	std::ifstream file;
	// 	file.open("old/nginx.conf");
	// 	if (file.fail())
	// 	{
	// 			std::cout << "cant open file" << std::endl;
	// 	}
	// 	std::string line;
	// 	while (getline(file, line))
	// 	{
	// 			send(client_socket, line.c_str(), line.size(), 0);
	// 			send(client_socket, "\n", 1, 0);
	// 	}

    // send simple string
	// std::string response = "Good talking to you\n";
	// send(client_socket, response.c_str(), response.size(), 0);

	// Close the client_sockets
	// close(client_socket);
	// close(server_socket);
	return (0);
}