#ifndef WEBSERV_HPP
#define WEBSERV_HPP


#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib> // For exit() and EXIT_FAILURE
#include <iostream> // For cout
#include <unistd.h> // For read
#include <fstream> // ifstream
#include <sstream> // sstream


// Request
#include "GNL/get_next_line.h"
#include "Request/request_class.hpp"
int process_request(int connection, Request &req);


#endif