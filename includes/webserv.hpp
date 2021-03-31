#ifndef WEBSERV_HPP
#define WEBSERV_HPP


# include <string>
# include <iostream>

# include <sys/socket.h> // For socket functions
# include <netinet/in.h> // For sockaddr_in
# include <cstdlib> // For exit() and EXIT_FAILURE
# include <fstream> // ifstream
# include <fcntl.h> // For fcntl --> non_blocking fd
# include <map>

// #include <fstream> // ifstream
// #include <sstream> // sstream


// Colors
# define RED		"\033[31;1m"
# define GREEN		"\033[32;1m"
# define YELLOW		"\033[33;1m"
# define WHITE		"\033[37;1m"
# define NOCOLOR	"\033[0m"

#endif