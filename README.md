# webserv

compile: clang++ -Werror -Wall -Wextra --std=c++98 -fsanitize=address *.cpp GNL/*.cpp

connect to server: telnet localhost 9999
