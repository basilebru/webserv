#include "../webserv.hpp"


int crlf_gnl(int connection, std::string &line, Request &req)
{
	char *char_line;
	if (get_next_line(connection,&char_line) < 0)
	{
		std::cout << "problem reading from socket" << std::endl;
		return -1;
	}
	line = char_line;
	free(char_line);
	if (line[line.length() - 1] != '\r')
	{
		req.set_error_code(400);
		std::cout << "parsing error: expected \r\n at the end of a line" << std::endl;
	}
	else
		line.erase(line.length() - 1); // erase the /r at the end
	return 0;
}

void process_request(int connection, Request &req)
{
	// Read from connection : Read directly one line at a time (and proceed it directly) with our C-style getnextline

	bool go_on = true;
	std::string line;

	// 1. parsing et stockage de la request_line
	crlf_gnl(connection, line, req);
	if (line.empty()) // ignore one empty line before request (cf RFC 7230 3.5) -> pratique dans le cas ou on a un body et une content-length: on peut sauter une ligne entre la fin du body et la prochaine request (comme dans nginx)
		crlf_gnl(connection, line, req);
	req.add_req_line(line);
	if (req.get_error_code())
		return ;

	// 2. parsing et stockage des header fields
	while (go_on)
	{
		crlf_gnl(connection, line, req);	
        if (!line.empty())
            req.add_header(line);
		else
			go_on = false; // empty line signals the end of the header fields
		if (req.get_error_code())
			return ;
	}

	// 3. parsing des "body_headers" (Content-Length et Transfer-Enconding) et stockage du body
	// (seul "chunked" est en Transfer-Encoding)
	req.parse_body(connection);
	
}