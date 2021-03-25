#include "request_class.hpp"
#include <algorithm> // find functions
#include <cstdlib> // strtol


Request::Request(int fd): fd(fd), error_code(0), body_size(0), chunked_encoding(false)
{
}

// Request::Request(const Request &copy): req_line(copy.req_line), headers(copy.headers)
// {
// }

Request::~Request(void)
{
}

// Request&   Request::operator=(const Request &rhs)
// {
//     this->req_line = rhs.req_line;
//     this->headers = rhs.headers;
//     return(*this);
// }


int Request::parse()
{
    // std::cout << "parsing new request" << std::endl;
    this->parse_req_line();
    if (!this->error_code)
        this->parse_headers();
    if (!this->error_code)
        this->parse_body();
    return 1;
}

void Request::parse_req_line()
{
	std::string line;

	this->readline(line);
    //  ignore one empty line before request (cf RFC 7230 3.5) 
    //  -> pratique dans le cas ou on a un body et une content-length: on peut sauter une ligne entre la fin du body et la prochaine request (comme dans nginx)
	if (line.empty() && !this->error_code)
	    this->readline(line);
    if (!this->error_code)
	    this->store_req_line(line);
}

void Request::parse_headers()
{
	std::string line;
	
    while (1)
	{
	    this->readline(line);
        if (line.empty() || this->error_code)
            break ;
        
        this->store_header(line);
        if (this->error_code)
            break;
	}
}

void Request::parse_body()
{
    this->check_body_headers(); // will check the "content-length" and "transfer-encoding headers"
    if (this->error_code)
        return;
    
    if (this->chunked_encoding)
        this->read_chunked();
    else if (this->body_size)
        this->read_normal();
}

int Request::readline(std::string &line)
{
	char *char_line;
	int ret = 0;

	ret = get_next_line(this->fd, &char_line);
	if (ret < 0)
	{
		this->error_message = "internal-error: problem reading from socket";
        this->error_code = 500;
		return ret;
	}
	line = char_line;
	free(char_line);
	if (line[line.length() - 1] != '\r')
	{
		this->error_code = 400;
		this->error_message = "parsing error: expected CRLF at the end of a line";
	}
	else
		line.erase(line.length() - 1); // erase the /r at the end
    return ret;
}

void Request::read_normal() // we read the number of bytes specified by "content-lenght"
{
        int ret(0);
        int received(0);
		char *body = (char*)malloc(this->body_size + 1);
		if (body == NULL)
		{
			this->error_message = "internal-error: alloc problem reading body";
            this->error_code = 500;
			return ;
		}
        // we can't assume that we will be able to read the whole body in one call: we should be prepared for interuptions (eg: pressing "Enter" makes read() return) = a read can return only part of the desired number of bytes
        while (received != this->body_size)
        {
            // std::cout << "body_size - received " << this->body_size - received << std::endl;
		    ret = read(this->fd, body, this->body_size - received);
            body[ret] = 0;
            // std::cout << "ret: " << ret << std::endl;
            received += ret;
            // std::cout << "received: " << received << std::endl;
            this->body += body;
        }
		free(body);
}

void Request::read_chunked()
{
    // read chunk-size and CRLF
    // while (chunk-size > 0)
        // read chunk_data and CRLF
        // body += chunk_data
        // content-length += chunk_size
        // read chunk_size and CRLF

    long int chunk_size;
    char *line;
    long int ret;
    long int received;

    get_next_line(this->fd, &line);
    chunk_size = std::strtol(line, NULL, 16);
    while (chunk_size)
    {
        free(line);
        line = (char*)malloc(chunk_size + 1);

        // same as read_normal: we cant assume that read() will be able to read the whole chunk in one call
        received = 0;
        while (received != chunk_size)
        {
            // std::cout << "chunk_size - received " << chunk_size - received << std::endl;
            ret = read(this->fd, line, chunk_size - received);
            // std::cout << "ret: " << ret << std::endl;
            line[ret] = 0;
            received += ret;
            // std::cout << "received: " << received << std::endl;
            this->body += line;
        }
        
        ret = read(this->fd, line, 2); // read CRLF
        if (ret != 2 || line[0] != '\r' || line[1] != '\n') // make sure it is CRLF
        {
            free(line);
            this->error_message = "parsing error: no CRLF after chunked data";
            this->error_code = 400;
            return ;
        }
        free(line);

        get_next_line(this->fd, &line);
        chunk_size = std::strtol(line, NULL, 16);
    }
    ret = read(this->fd, line, 2); // read CRLF
    if (ret != 2 || line[0] != '\r' || line[1] != '\n') // make sure it is CRLF
    {
        this->error_message = "parsing error: no CRLF after chunked data";
        this->error_code = 400;
    }
    free(line);
}


void Request::store_req_line(std::string line)
{
    size_t count(0);
    size_t pos(0);
    size_t next;
    // we split on whitespaces, since "no whitespace is allowed in the three components" and a single space is required between components
    if (std::count(line.begin(), line.end(), ' ') != 2)
    {
        this->error_message = "parsing error: request line not well formated: " + line;
        this->error_code = 400;
        return ;
    }
    while((next = line.find(" ", pos)) != std::string::npos)
    {
        //std::cout << "pos: " << pos << std::endl;
        //std::cout << "next: " << next << std::endl;
        // std::cout << "sub: " << line.substr(pos, next - pos) << std::endl;
        if (count == 0)
            this->req_line.method = line.substr(pos, next - pos);
        else // count == 1
            this->req_line.target = line.substr(pos, next - pos);
        pos = next + 1;
        count++;
    }
    this->req_line.version = line.substr(pos);
}

void Request::store_header(std::string line)
{
    size_t find;
    std::string field_name;
    std::string field_value;
    
    if (is_whitespace(line[0]))
    {
        this->error_message = "parsing error: header line starts with whitespace: " + line;
        this->error_code = 400;
        return ;
    }
    if ((find = line.find(':')) == std::string::npos)
    {
        this->error_message = "parsing error: no double colon in header: " + line;
        this->error_code = 400;
        return ;
    }
    field_name = line.substr(0, find);
    if (is_whitespace(field_name[field_name.size() - 1]))
    {
        this->error_message = "parsing error: whitespace before double colon: " + line;
        this->error_code = 400;
        return ;
    }
    field_value = line.substr(find + 1);
    trim_whitespace(field_value);
    this->headers.push_back(header(field_name, field_value));
}

void Request::check_body_headers()
{
    if (this->has_transfer_encoding())
    {
        this->store_encoding();
        return ; // priorité au transfer-encoding header sur le body-length header
    }
    if (this->has_content_length())
        this->store_body_length();
}

bool Request::has_transfer_encoding()
{
    if (std::find_if(this->headers.begin(), this->headers.end(), transfer_encoding_present) != this->headers.end())
        return true;
    return false;
}

bool Request::has_content_length()
{
    if (std::find_if(this->headers.begin(), this->headers.end(), content_length_present) != this->headers.end())
        return true;
    return false;
}

void Request::store_encoding()
{
    std::string encoding;
    encoding =  std::find_if(this->headers.begin(), this->headers.end(), transfer_encoding_present)->second;
    if (encoding == "chunked" || encoding == "Chunked")
        this->chunked_encoding = true;
    else
    {
        this->error_message = "parsing error: transfer-encoding name not supported: " + encoding;
        this->error_code = 500;
    }
}

void Request::store_body_length()
{
    std::string body_length;
    body_length =  std::find_if(this->headers.begin(), this->headers.end(), content_length_present)->second;
    if (ft_isdigit_str(body_length.c_str()) == false)
    {
        this->error_message = "parsing error: Content-length header value is invalid: " + body_length;
        this->error_code = 400;
        return ;
    }
    if (body_length.length() > 7) // > 9.9999 MB
    {
        this->error_message = "parsing error: Content-length header value is too big: " + body_length;
        this->error_code = 400;
        return ;
    }
    int ret(0);
    int i(0);
	while (body_length[i])
	{
		ret = ret * 10 + body_length[i] - 48;
		i++;
	}
    this->body_size = ret;
    // std::cout << "body length is: " << this->body_size << std::endl;
}

int Request::get_error_code() const
{
    return this->error_code;
}

// void Request::set_error_code(int code)
// {
//     this->error_code = code;
// };

void Request::print()
{
    if (this->error_code)
    {
        std::cout << "Error:" << error_code << std::endl;
        std::cout << error_message << std::endl;
        return ;
    }
    std::cout << "Request line:" << std::endl;
    std::cout << " . Method: " << this->req_line.method << std::endl;
    std::cout << " . Target: " << this->req_line.target << std::endl;
    std::cout << " . Version: " << this->req_line.version << std::endl;
    std::cout << std::endl;
    for (std::list<header>::iterator it = this->headers.begin(); it != this->headers.end(); it++)
    {
        /* code */
        std::cout << "Header line:" << std::endl;
        std::cout << " . field_name: " << "[" << it->first << "]" << std::endl;
        std::cout << " . field_value: " << "[" << it->second << "]" << std::endl;
        std::cout << std::endl;
    }
    if (!this->body.empty())
    {
        std::cout << "body: " << this->body << std::endl;
        std::cout << std::endl;
    }
}

bool content_length_present(std::pair<std::string, std::string> header)
{
    if (header.first == "Content-Length" || header.first == "content-length")
        return true;
    else
        return false;
}

bool transfer_encoding_present(std::pair<std::string, std::string> header)
{
    if (header.first == "Transfer-Encoding" || header.first == "transfer-encoding")
        return true;
    else
        return false;
}

