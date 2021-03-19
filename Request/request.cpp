#include "../webserv.hpp"

void process_request(int connection, Request &req)
{
	// Read from connection : Read directly one line at a time (and proceed it directly) with our C-style getnextline

	bool go_on = true;
	char *char_line;
    int line_num(0);
	std::string string_line;

	// 1. pasing et stockage de la request_line et des header fields
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
	
	// 2. parsing des "body_headers" et si nécessaire du body
	req.parse_body(connection);
	
}