#include "request_class.hpp"

Request::Request(int fd, sockaddr_in addr, const std::vector<ServerBlock> &servers, const HttpBlock &base_config) :
fd(fd), error_code(0), address(addr), servers(servers), base_config(base_config)
{
    this->end_of_connection = false;
    this->chunked_encoding = false;
    this->req_line_read = false;
    this->request_ready = false;
    this->chunked_size_read = false;
    this->error_code = 0;
    this->body_size = 0;
    this->chunk_size = 0;
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

void Request::match_server()
{
    std::vector<ServerBlock> eligible_servers;
    // 1. evaluate IP and port
    // 1.a try exact match
    for (std::vector<ServerBlock>::const_iterator it = this->servers.begin(); it != this->servers.end(); it++)
    {
        if (it->getListenIP() && it->getListenIP() == this->address.sin_addr.s_addr && it->getListenPort() == this->address.sin_port)
            eligible_servers.push_back(*it);
    }
    // 1.b if no exact match, try 0.0.0.0 match 
    if (eligible_servers.size() == 0)
    {
        for (std::vector<ServerBlock>::const_iterator it = this->servers.begin(); it != this->servers.end(); it++)
        {
            if (it->getListenIP() == 0 && it->getListenPort() == this->address.sin_port)
                eligible_servers.push_back(*it);
        }
    }
    // if only one match, chose this server
    if (eligible_servers.size() == 1)
    {
        this->matched_serv = eligible_servers[0];
        return ;
    }

    // 2. if multiple matchs, evaluate server_name
    // chose first server_block that matches
    for (std::vector<ServerBlock>::iterator it = eligible_servers.begin(); it != eligible_servers.end(); it++)
    {
        if (std::find(it->getServerNames().begin(), it->getServerNames().end(), this->host_uri) != it->getServerNames().end())
        {
            this->matched_serv = *it;
            return ;
        }
    }
    // if no match, chose first server_block on the list
    this->matched_serv = eligible_servers[0];

}

void Request::match_location()
{
    std::string target_uri(this->req_line.target);
    while (target_uri.find('/') != std::string::npos)
    {
        for (LocMap::const_iterator it = this->matched_serv.getLocations().begin(); it != this->matched_serv.getLocations().end(); it++)
        {
            if (target_uri.compare(it->first) == 0) // compare target uri and location "path"
            {
                this->matched_loc = it->second;
                return ;
            }
        }
        target_uri = target_uri.substr(0, target_uri.find_last_of('/')); // cut target_uri at last '/'
    }
}

void Request::fill_conf()
{
    this->match_server(); // this->matched_serv is filled
    std::cout << "----" << std::endl;
    std::cout <<"matched serv: " << this->matched_serv.getServerNames()[0] << std::endl;
    this->match_location(); // this->matched_loc is filled -- if no match is found, this->matched_loc is left as it is (object default constructor: attributes "unset"
    std::cout << "matched loc: " << this->matched_loc.getPath() << std::endl;
    // for each attribute: try fill it with location block. if directive not set in location block, use Server bloc. if directive not set in server block, use http bloc (default value for directive)

    this->config.allow_methods = this->matched_loc.getLimitExcept();
    if (this->config.allow_methods.empty())
        this->config.allow_methods = this->matched_serv.getLimitExcept();
    if (this->config.allow_methods.empty())
        this->config.allow_methods = this->base_config.getLimitExcept();

    this->config.autoindex = this->matched_loc.getAutoindex();
    if (this->config.autoindex == NOT_SET)
        this->config.autoindex = this->matched_serv.getAutoindex();
    if (this->config.autoindex == NOT_SET)
        this->config.autoindex = this->base_config.getAutoindex();

    this->config.error_pages = this->matched_loc.getErrorPages();
    if (this->config.error_pages.empty())
        this->config.error_pages = this->matched_serv.getErrorPages();
    if (this->config.error_pages.empty())
        this->config.error_pages = this->base_config.getErrorPages();
    
    this->config.index = this->matched_loc.getIndexes();
    if (this->config.index.empty())
        this->config.index = this->matched_serv.getIndexes();
    if (this->config.index.empty())
        this->config.index = this->base_config.getIndexes();
    
    this->config.max_body_size = this->matched_loc.getMaxBdySize();
    if (this->config.max_body_size == NOT_SET)
        this->config.max_body_size = this->matched_serv.getMaxBdySize();
    if (this->config.max_body_size == NOT_SET)
        this->config.max_body_size = this->base_config.getMaxBdySize();

    this->config.root = this->matched_loc.getRoot();
    if (this->config.root.empty())
        this->config.root = this->matched_serv.getRoot();
    if (this->config.root.empty())
        this->config.root = this->base_config.getRoot();
}

void Request::init_config()
{
    // get host header (needed to find config)
    this->store_host();
    if (this->error_code)
        return ;
    
    // retrieve config
    this->fill_conf();
    // std::cout << "maxbody size: " << this->config.max_body_size << std::endl;

    // "max_body_size": check the "content-length" and "transfer-encoding headers" --> in store_body_headers()
    this->store_body_headers(); 
    if (this->error_code)
        return ;
    
    // "allow methods": check req_line.method
    if (std::find(this->config.allow_methods.begin(), this->config.allow_methods.end(), this->req_line.method) == this->config.allow_methods.end())
    {
        this->error_message = "method not allowed: " + this->req_line.method;
        this->error_code = 405;
        return ;
    }

    // "root": build target_uri
    if (this->config.root[this->config.root.size() - 1] == '/') // delete '/' at the end of root (if present)
        this->config.root.erase(this->config.root.end() - 1);
    this->target_uri = this->config.root + this->req_line.target;
    std::cout << "[target uri:] " << this->target_uri << std::endl;
    std::cout << "----" << std::endl;
}
