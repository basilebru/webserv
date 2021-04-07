#include "request_class.hpp"

Request::Request(int fd): fd(fd), error_code(0), end_of_connection(false)
{
    // error_code and end_of_connection are not set in Request::reset, as they imply the end of the connection
    this->error_code = 0;
    this->end_of_connection = false;

    this->reset();
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

    // first we read from socket (as much as we can), and store what we've read into a buffer
    // then we parse this buffer:
    // 1. read req_line
    // 2. read_headers
    // 3. find configuration block, import config
    // 4. read body
    // notes:
    //  ->  while parsing the buffer, we check that the request synthax is ok and compliant with configuration directives
    //  ->  If the buffer does not contain the whole request, we return back to select() and wait for new data to read()
    //      so we should be able to pick up where we left off

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

    this->init_config();
    if (this->error_code)
        return ;

    this->parse_body();
}

void Request::parse_body()
{
    std::cout << "Parsing body..." << std::endl;

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
            if (this->parse_chunked_data() == false)
                return ;
            if (this->chunk_size == 0) // end of request, stop reading
            {
                this->request_ready = true;
                return ;
            }
            this->chunked_size_read = false; // go on reading new chunk size
        }
        std::cout << "Parsing chunked size..." << std::endl;
        if (this->parse_chunked_size() == false) // not enough data in buffer
            return ;
    }
}

bool Request::parse_chunked_size()
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

bool Request::parse_chunked_data()
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

void Request::init_config()
{
    // get host header (needed to find config)
    this->store_host();
    if (this->error_code)
        return ;
    
    // retrieve config
    
    this->max_body_size = 10;
    this->root = "html";
    // this->index.push_back("index.html");
    this->allow_methods.push_back("GET");
    this->allow_methods.push_back("POST");

    // check that request is ok with config:
    
    // "max_body_size": check the "content-length" and "transfer-encoding headers"
    this->store_body_headers(); 
    if (this->error_code)
        return ;
    
    // "allow methods": check req_line.method
    if (std::find(allow_methods.begin(), allow_methods.end(), this->req_line.method) == allow_methods.end())
    {
        this->error_message = "method not allowed: " + this->req_line.method;
        this->error_code = 405;
        return ;
    }

    // build target_uri
    if (this->root[this->root.size() - 1] == '/')
        this->root.erase(this->root.end() - 1);
    this->target_uri = this->root + this->req_line.target;
    std::cout << "[target uri:] " << this->target_uri << std::endl;

}
