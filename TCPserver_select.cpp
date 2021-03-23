#include "webserv.hpp"
#include <csignal>

int main() {


	fd_set current_sockets, ready_sockets;
	int max_sockets = 0;

	// Create a socket (IPv4, TCP)
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == -1) {
		std::cout << "Failed to create socket. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}


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


	//Optional: sets the timeout for select()
	struct timeval timeout;

    // process requests until an error is found on a request
	std::string request_ok = "Request received :)\n";
	std::string request_ko = "Request error :( \n";
	int connection;
	size_t addrlen = sizeof(sockaddr);
	while (1)
	{
		//Optional: sets the timeout for select()
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;

		// select is destructive so we need a socket copy
		ready_sockets = current_sockets;
		int sret = select(FD_SETSIZE, &ready_sockets, NULL, NULL, &timeout);
		if (sret < 0)
		{
			std::cerr << "Select error. errno: " << errno << std::endl;
			exit(EXIT_FAILURE);
		}
		else if (sret == 0)
			std::cerr << sret << ": Select timeout (5 sec)" << std::endl;

		// Loop over the FD_SET
		for (int i = 0; i < FD_SETSIZE; ++i)
		{
			if (FD_ISSET(i, &ready_sockets))
			{
				if (i == server_socket)
				{
					connection = accept(server_socket, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
					if (connection < 0) {
						std::cout << "Failed to grab connection. errno: " << errno << std::endl;
						exit(EXIT_FAILURE);
					}
					FD_SET(connection, &current_sockets);
				}
				else
				{
					Request *req = new Request;
					process_request(i, *req);
					// FD_CLR(i, &current_sockets);
			
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

	
	// Send a message to the connection

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
	// 			send(connection, line.c_str(), line.size(), 0);
	// 			send(connection, "\n", 1, 0);
	// 	}

    // send simple string
	// std::string response = "Good talking to you\n";
	// send(connection, response.c_str(), response.size(), 0);

	// Close the connections
	close(connection);
	close(server_socket);
	return (0);
}