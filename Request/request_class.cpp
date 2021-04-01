#include "request_class.hpp"
#include <algorithm> // find functions
#include <cstdlib> // strtol
#include <sys/socket.h> // recv
#include <cstring> // strcmp

Request::Request(int fd): fd(fd), error_code(0), body_size(0), chunk_size(0), chunked_encoding(false), chunked_size_read(false), req_line_read(false), end_of_connection(false), request_ready(false)
{
}

// Request::Request(const Request &copy) 
// {
// }

Request::~Request(void)
{
}

// Request&   Request::operator=(const Request &rhs)
// {
// }

void Request::parse()
{
    try
    {
        this->read_from_socket();
        if (this->error_code)
            return;
        std::cout << "buffer: " << this->buffer << std::endl;
        this->parse_buffer();
    }
    catch(const std::exception& e)
    {
        this->error_code = 500;
		this->error_message = "internal-error: " + std::string(e.what());
    }
    
}

void Request::read_from_socket()
{

    // TO DO:
    // - set a limit on buffer size ?

    long int ret;
    char *buf;
    buf = (char*)malloc(BUF_SIZE + 1);
    if (buf == NULL)
    {
		this->error_message = "internal-error: malloc error";
		this->error_code = 500;
    }
    while ((ret = recv(this->fd, buf, BUF_SIZE, MSG_DONTWAIT)) > 0)
    {
        buf[ret] = 0;
        if (ret == 5 && strcmp(buf, "\xFF\xF4\xFF\xFD\x06") == 0)
            this->error_code = 400;
        this->buffer += buf;
    }
    free(buf);
    if (ret == 0)
        this->end_of_connection = true;
}


void Request::parse_buffer()
{
    std::cout << "Parsing buffer..." << std::endl;
    if (this->chunked_encoding || this->body_size)
        this->parse_body();
    else if (this->req_line_read)
        this->parse_headers();
    else
        this->parse_req_line();
}

bool Request::read_buf_line(std::string &line)
{
    size_t pos;
    pos = this->buffer.find("\r\n");
    if (pos == std::string::npos)
        return false;
    line = this->buffer.substr(0, pos);
    this->buffer.erase(0, pos + 2);
    return true;
}

void Request::parse_req_line()
{
    std::cout << "Parsing req_line..." << std::endl;
    std::string line;
    bool line_read;
    line_read = this->read_buf_line(line);
    while (line_read && line.empty()) // skip empty line(s) before req_line
        line_read = this->read_buf_line(line);
    if (line_read)
    {
        this->store_req_line(line);
        if (this->error_code)
            return ;
        this->req_line_read = true;
        this->parse_headers();
    }
}

void Request::parse_headers()
{
    std::cout << "Parsing header..." << std::endl;
    std::string line;
    bool line_read;

    while (1)
    {
        line_read = this->read_buf_line(line);
        if (!line_read) // if no CRLF was found in buf, quit parsing
            return ;
        if (line.empty())
            break ;
        this->store_header(line);
        if (this->error_code)
            return ;
    }
    this->parse_body();
}

void Request::parse_body()
{
    std::cout << "Parsing body..." << std::endl;
    this->check_body_headers(); // will check the "content-length" and "transfer-encoding headers"
    if (this->error_code)
        return ;

    if (!this->chunked_encoding)
        this->parse_body_normal();
    else
        this->parse_body_chunked();
}

void Request::parse_body_normal()
{
    std::cout << "Parsing body normal..." << std::endl;
    if (this->buffer.size() >= this->body_size)
    {
        this->body = this->buffer.substr(0, this->body_size);
        this->buffer.erase(0, this->body_size);
        this->request_ready = true;
    }
}

void Request::parse_body_chunked()
{
    std::cout << "Parsing body chunked..." << std::endl;
    std::string line;
    while (1)
    {
        if (this->chunked_size_read)
        {
            std::cout << "Parsing chunked data..." << std::endl;
            if (this->read_chunked_data() == false)
                return ;
            if (this->chunk_size == 0) // end of request, stop reading
            {
                this->request_ready = true;
                return ;
            }
            this->chunked_size_read = false; // go on reading new chunk size
        }
        std::cout << "Parsing chunked size..." << std::endl;
        if (this->read_chunked_size() == false) // not enough data in buffer
            return ;
    }
}

bool Request::read_chunked_size()
{
    std::string line;
    if (this->read_buf_line(line) == false)
        return false;
    this->store_chunk_size(line);
    if (this->error_code)
        return false;
    this->chunked_size_read = true;
    return true;
}

bool Request::read_chunked_data()
{
    if (this->buffer.size() < this->chunk_size + 2) // + 2 because we also want to read crlf after chunk data
        return false;
    this->body += this->buffer.substr(0, this->chunk_size);
    this->buffer.erase(0, this->chunk_size);
    std::string crlf = this->buffer.substr(0, 2);
    if (crlf != "\r\n")
    {
        this->error_message = "parsing error: no CRLF after chunked data";
        this->error_code = 400;
        return false;
    }
    this->buffer.erase(0, 2);
    return true;
}


void Request::reset()
{
    std::cout << "reseting request" << std::endl;
    // set bools to false
    this->chunked_encoding = false;
    this->req_line_read = false;
    this->request_ready = false;
    this->chunked_size_read = false;
    // reset req line, headers & body
    this->req_line.method = "";
    this->req_line.version = "";
    this->req_line.target = "";
    this->headers.erase(this->headers.begin(), this->headers.end());
    this->body = "";
    this->body_size = 0;
    this->chunk_size = 0;
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

bool Request::has_transfer_encoding() const
{
    if (std::find_if(this->headers.begin(), this->headers.end(), transfer_encoding_present) != this->headers.end())
        return true;
    return false;
}

bool Request::has_content_length() const
{
    if (std::find_if(this->headers.begin(), this->headers.end(), content_length_present) != this->headers.end())
        return true;
    return false;
}


void Request::store_chunk_size(std::string line)
{
    if (ft_isxdigit_str(line.c_str()) == false)
    {
        this->error_message = "parsing error: chunk size value is invalid: " + line;
        this->error_code = 400;
        return ;
    }
    if (line.length() > 6) // > FFFFFF = 1,049 MB
    {
        this->error_message = "parsing error: chunk-size value is too big: " + line;
        this->error_code = 400;
        return ;
    }
    this->chunk_size = strtol(line.c_str(), NULL, 16);
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

bool Request::connection_end() const
{
    return this->end_of_connection;
}

bool Request::request_is_ready() const
{
    return this->request_ready;
}

// void Request::set_error_code(int code)
// {
//     this->error_code = code;
// };

void Request::print() const
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
    for (std::list<header>::const_iterator it = this->headers.begin(); it != this->headers.end(); it++)
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

