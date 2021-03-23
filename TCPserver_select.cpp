#include "webserv.hpp"
#include <csignal>
#include <fcntl.h>

int main() {


	fd_set current_sockets, ready_sockets;
	int max_sockets = 0;

	// Create a socket (IPv4, TCP)
	// Using the flag SOCK_NONBLOCK saves extra calls to fcntl(2) to achieve the same result.
	int server_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0); 
	if (server_socket == -1) {
		std::cout << "Failed to create socket. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}

	// Set the socket as NON BLOCKING (for MAC, no need in Linux ?)
	if (fcntl(server_socket, F_SETFL, O_NONBLOCK) < 0)
		std::cout << "Fcntl failed. errno: " << errno << std::endl;

	// In order to reuse the socket quickly after stopping and restarting the server
	// Don't take TIME_WAIT into consideration
	int optval = 1;
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
		std::cout << "Error: setsockopt(SO_REUSEADDR) failed. errno: " << errno << std::endl;


	// Listen to port 9999 on any address
	sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons(9999); // htons is necessary to convert a number to
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


	//Initilaize the current socket set
	FD_ZERO(&current_sockets);
	FD_SET(server_socket, &current_sockets);
	max_sockets = server_socket;
	printf("server_socket: %d\n", max_sockets);
	// printf("FD_SETSIZE: %d\n", FD_SETSIZE);

	//Optional: sets the timeout for select()
	struct timeval timeout;

    // process requests until an error is found on a request
	std::string request_ok = "Request received :)\n";
	std::string request_ko = "Request error :( \n";
	int client_socket;
	size_t addrlen = sizeof(sockaddr);
	while (1)
	{
		//Optional: sets the timeout for select()
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;

		// select is destructive so we need a socket copy
		ready_sockets = current_sockets;
		int sret = select(max_sockets + 1, &ready_sockets, NULL, NULL, &timeout);
		if (sret < 0)
		{
			std::cerr << "Select error. errno: " << errno << std::endl;
			exit(EXIT_FAILURE);
		}
		else if (sret == 0)
			std::cerr << sret << ": Select timeout (5 sec)" << std::endl;

		// Loop over the FD_SET
		for (int i = 0; i < max_sockets + 1; ++i)
		{
			if (FD_ISSET(i, &ready_sockets))
			{
				printf("socket used: %d\n", i);
				if (i == server_socket)
				{
					client_socket = accept(server_socket, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
					if (client_socket < 0) {
						std::cout << "Failed to grab connection. errno: " << errno << std::endl;
						exit(EXIT_FAILURE);
					}
					FD_SET(client_socket, &current_sockets);
					if (client_socket > max_sockets)
						max_sockets = client_socket;
				}
				else
				{
					Request *req = new Request;
					int ret = process_request(i, *req);
					if (ret == 0)
					{
						close(i);
						FD_CLR(i, &current_sockets);
					}

					// send() errors handling
					if ((*req).get_error_code())
						send(i, request_ko.c_str(), request_ko.size(), 0);
					else
						send(i, request_ok.c_str(), request_ok.size(), 0);
					(*req).print();
					if ((*req).get_error_code())
					{
						delete req;
						break;
					}
					delete req;
				}
			}
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
	close(client_socket);
	close(server_socket);
	return (0);
}