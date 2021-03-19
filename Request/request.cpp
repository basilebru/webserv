#include "../webserv.hpp"

void process_request(int connection, Request &req)
{
	// Read from connection : Read directly one line at a time (and proceed it directly) with our C-style getnextline

	bool go_on = true;
	char *char_line;
    int line_num(0);
	std::string string_line;

	// 1. parsing de la request_line et des header fields
	while (go_on)
	{
		if (get_next_line(connection,&char_line) < 0)
			std::cout << "cant read from socket" << std::endl;
		string_line = char_line;
		string_line.erase(string_line.length() - 1); // erase the /r at the end
		go_on = !string_line.empty();
        if (line_num == 0)
            req.add_req_line(string_line);
        else if (go_on)
            req.add_header(string_line);
        free(char_line);
        line_num++;
	}
	req.print();
	
	// 2. parsing du body (si nécessaire)
	req.parse_body_headers();
	if (req.get_error_code())
		return ;
	if (req.get_body_lenght() != -1)
	{
		char *body = (char*)malloc(req.get_body_lenght() + 1);
		read(connection, body, req.get_body_lenght());
		body[req.get_body_lenght()] = 0;
		std::cout << "body is: " << body << std::endl;
		std::cout << std::endl;
		read(connection, body, 2); // read CRLF
		free(body);
	}
	
	// Alternative:
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