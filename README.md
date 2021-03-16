# webserv

compile: clang++ -Werror -Wall -Wextra --std=c++98 -fsanitize=address TCPserver.cpp GNL/get_next_line_utils.cpp GNL/get_next_line.cpp

connect to server: telnet localhost 9999
