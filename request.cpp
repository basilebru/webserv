#include "webserv.hpp"

void process_request(int connection)
{
	// Read from connection
	// A. Read directly one line at a time (and proceed it directly) with our C-style getnextline

	bool go_on = true;
	char *char_line;
    int line_num(0);
	std::string string_line;
    Request req;

	while (go_on)
	{
		if (get_next_line(connection,&char_line) < 0)
			std::cout << "cant read from socket" << std::endl;
		string_line = char_line;
		std::cout << "request_line: " << string_line << std::endl;
        // if (line_num == 0)
            // process request line
        // else
            // process header lines
		go_on = string_line != "\r";
        free(char_line);
        line_num++;
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

}