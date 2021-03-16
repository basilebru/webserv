#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib> // For exit() and EXIT_FAILURE
#include <iostream> // For cout
#include <unistd.h> // For read
#include <fstream> // ifstream
#include <sstream> // sstream
#include <stdio.h> // fdopen
#include "GNL/get_next_line.h"

int main() {
	// Create a socket (IPv4, TCP)
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		std::cout << "Failed to create socket. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}

	// Listen to port 9999 on any address
	sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons(9999); // htons is necessary to convert a number to
																	 // network byte order
	if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
		std::cout << "Failed to bind to port 9999. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
// Start listening. Hold at most 10 connections in the queue
	if (listen(sockfd, 10) < 0) {
		std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}

	// Grab a connection from the queue
	unsigned long addrlen = sizeof(sockaddr);
	int connection = accept(sockfd, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
	if (connection < 0) {
		std::cout << "Failed to grab connection. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}


	// Read from connection
	// A. Read directly one line at a time (and proceed it directly) with our C-style getnextline

	bool go_on = true;
	char *char_line;
	std::string string_line;
	while (go_on)
	{
		if (get_next_line(connection,&char_line) < 0)
			std::cout << "cant read from socket" << std::endl;
		string_line = char_line;
		std::cout << "request_line: " << string_line << std::endl;
		// process line
		go_on = string_line != "\r";
        free(char_line);
	}
	
	// B. Alternative:
	// Read from the connection until an empty line is found. Concatenate the successive buffers into a stringstream
	// Then call getlines on stringstream and proceed each line

	// char buffer[256];
	// std::stringstream data;
	// bool go_on = true;
	// int len;
	// while (go_on)
	// {
	// 	len = read(connection, buffer, 255);
	// 	if (len < 0)
	// 	{
	// 		std::cout << "cant read from socket" << std::endl;
	// 	}
	// 	buffer[len] = 0;
	// 	// std::cout << "buf: " << buffer << std::endl;
	// 	// std::cout << "len: " << len << std::endl;
	// 	data << buffer;
	// 	go_on = data.str().find("\r\n\r\n") == std::string::npos;
	// }
	// std::cout << data.str() << std::endl;
		// std::string my_line;
		// while (getline(data, my_line))
		// {
		// 	std::cout << "request line:" << my_line << std::endl;
		// }

	
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
	std::string response = "Good talking to you\n";
	send(connection, response.c_str(), response.size(), 0);

	// Close the connections
	close(connection);
	close(sockfd);
}