#ifndef WEBSERV_HPP
# define WEBSERV_HPP


# include <string>
# include <iostream>

# include <sys/socket.h> // For socket functions
# include <sys/select.h>
# include <sys/stat.h>  // stat
# include <sys/types.h> // stat
# include <netinet/in.h> // For sockaddr_in
# include <fstream> // ifstream
# include <fcntl.h> // For fcntl --> non_blocking fd
# include <map>
# include <vector>
# include <list>

# include <sstream>
# include <bitset>

# include <cstring> // strcmp
# include <cstdlib> // exit(), EXIT_FAILURE, atoi(char*), strtol
# include <unistd.h>

# include <algorithm> //find function and co.


// Utils
# define TRUE		1
# define FALSE		0
# define SUCCESS	0
# define FAILURE	-1
# define ON			1
# define OFF		0
# define NOT_SET	-1

// Response return value
#define CLOSE -1
#define NEW 1
#define SKIP 0

// Default server config values
# define DEFAULT_INDEX			"index.html"
# define DEFAULT_AUTOINDEX		0				// OFF
# define DEFAULT_MAX_BDY_SIZE	1000000			// 1MB
# define DEFAULT_KEEPALIVE_T	75				// seconds
# define DEFAULT_LISTEN_IP		0				//"0.0.0.0"
# define DEFAULT_LISTEN_PORT	80
# define DEFAULT_SRV_NAME		""
# define DEFAULT_ROOT			"html"
# define DEFAULT_AUTH_BASIC		"off"
# define DEFAULT_CHUNKED_ENC	1				// ON
# define MAX_TCP_PORT			65535
# define DEFAULT_CONF_FILE		"./conf.d/webserv.conf"
# define DEFAULT_SERVER			"./conf.d/default.conf"

//CARRIAGE AND NEWLINES
# define CR		"\r"
# define LF		"\n"
# define CRLF	"\r\n"
# define CRLFX2	"\r\n\r\n"


// Colors
# define RED		"\033[31;1m"
# define GREEN		"\033[32;1m"
# define YELLOW		"\033[33;1m"
# define WHITE		"\033[37;1m"
# define NOCOLOR	"\033[0m"

#endif