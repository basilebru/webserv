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
        if (this->error_code || this->end_of_connection)
        {
            this->config.error_pages = this->base_config.getErrorPages();
            return;
        }
        // this->print_buffer(); //--> commenté pour faciliter le testeur
        // this->print(); //--> commenté pour faciliter le testeur
        this->parse_buffer();
    }
    catch(const std::exception& e)
    {
        this->error_code = 500;
		this->error_message = "internal-error: " + std::string(e.what());
    }
    if (this->error_code && this->config.error_pages.empty()) // if error code and config not set -> fill error_pages
        this->config.error_pages = this->base_config.getErrorPages();
}

void Request::read_from_socket()
{

    // TO DO:
    // - set a limit on buffer size ?

    long int ret;
    std::vector<unsigned char> buf(BUF_SIZE + 1);
    ret = recv(this->fd, &buf[0], BUF_SIZE, MSG_DONTWAIT);
    if (ret > 0)
    {
        buf.resize(ret);
        if (ret == 5 && buf == Request::ctrl_c)
            this->end_of_connection = true;
        this->buffer.insert(this->buffer.end(), buf.begin(), buf.begin() + ret);
    }
    if (ret == 0)
        this->end_of_connection = true;
}


void Request::parse_buffer()
{
    // std::cout << "Parsing buffer..." << std::endl;
    if (this->chunked_encoding || this->body_size)
        this->parse_body();
    else if (this->req_line_read)
        this->parse_headers();
    else
        this->parse_req_line();
}

void Request::parse_req_line()
{
    // std::cout << "Parsing req_line..." << std::endl;
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
    // std::cout << "Parsing header..." << std::endl;
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
    // std::cout << "Parsing body..." << std::endl;

    if (!this->chunked_encoding)
        this->parse_body_normal();
    else
        this->parse_body_chunked();
}

void Request::parse_body_normal()
{
    // std::cout << "Parsing body normal..." << std::endl;
    if (this->buffer.size() >= this->body_size)
    {
        this->body.assign(this->buffer.begin(), this->buffer.begin() + this->body_size);
        this->buffer.erase(this->buffer.begin(), this->buffer.begin() + this->body_size);
        this->request_ready = true;
    }
}

void Request::parse_body_chunked()
{
    // std::cout << "Parsing body chunked..." << std::endl;
    std::string line;
    while (1)
    {
        if (this->chunked_size_read)
        {
            // std::cout << "Parsing chunked data..." << std::endl;
            if (this->parse_chunked_data() == false)
            {
                // std::cout << "waiting for more data" << std::endl;
                return ;
            }
            if (this->chunk_size == 0) // end of request, stop reading
            {
                this->request_ready = true;
                return ;
            }
            this->chunked_size_read = false; // go on reading new chunk size
        }
        // std::cout << "Parsing chunked size..." << std::endl;
        if (this->parse_chunked_size() == false) // not enough data in buffer
        {
            // std::cout << "waiting for more data" << std::endl;
            return ;
        }
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
    this->body.insert(this->body.end(), this->buffer.begin(), this->buffer.begin() + this->chunk_size);
    this->buffer.erase(this->buffer.begin(), this->buffer.begin() + this->chunk_size);
    if (this->buffer.size() < 2 || this->buffer[0] != '\r' || this->buffer[1] != '\n')
    {
        this->error_message = "parsing error: no CRLF after chunked data";
        this->error_code = 400;
        return false;
    }
    this->buffer.erase(this->buffer.begin(), this->buffer.begin() +2);
    return true;
}

std::vector<unsigned char>::iterator Request::find_crlf()
{
    for (std::vector<unsigned char>::iterator it = this->buffer.begin(); it != this->buffer.end();)
    {
        if (*it == '\r')
        {
            it++;
            if (it == this->buffer.end())
                return this->buffer.end();
            if (*it == '\n')
                return --it;
        }
        else
            it++;
    }
    return this->buffer.end();
    
}

bool Request::read_buf_line(std::string &line)
{

    std::vector<unsigned char>::iterator pos = this->find_crlf();
    if (pos == this->buffer.end())
        return false;
    line.assign(this->buffer.begin(), pos);
    this->buffer.erase(this->buffer.begin(), pos + 2);
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

int Request::match_wildcard_locations()
{
    for (LocMap::const_iterator it = this->matched_serv.getLocations().begin(); it != this->matched_serv.getLocations().end(); it++)
    {
        size_t begining_of_extension;
        if (it->first.find('*') != std::string::npos)
        {
            bool location_has_extension = (begining_of_extension = it->first.find('.')) != std::string::npos;
            if (location_has_extension == false)
                continue;
            std::string location_extension = it->first.substr(begining_of_extension + 1);
            if (this->req_line.extension == location_extension)
            {
                this->matched_loc = it->second;
                return SUCCESS;
            }
        }
    }
    return FAILURE;

}

void delete_consecutive_slashes_in_uri(std::string &uri)
{
    for (unsigned long i = 0; uri.size() && i < uri.size() -1;)
    {
        if (uri[i] == '/' && uri[i + 1] == '/')
        {
            uri = uri.substr(0, i) + uri.substr(i + 1);
            i = 0;
        }
        else
            i++;
    }
}

void Request::match_location()
{
    // if (this->match_wildcard_locations() == SUCCESS)
    //     return ;

    std::string uri(this->req_line.target);
    delete_consecutive_slashes_in_uri(uri);

    while (uri.find('/') != std::string::npos)
    {
        for (LocMap::const_iterator it = this->matched_serv.getLocations().begin(); it != this->matched_serv.getLocations().end(); it++)
        {
            if (uri.compare(it->first) == 0) // compare target uri and location "path"
            {
                this->matched_loc = it->second;
                return ;
            }
        }
        uri = uri.substr(0, uri.find_last_of('/')); // cut uri at last '/'
    }
}

void Request::fill_conf()
{
    this->match_server(); // this->matched_serv is filled
    this->match_location(); // this->matched_loc is filled -- if no match is found, this->matched_loc is left as it is (object default constructor: attributes "unset"
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
    if (this->config.max_body_size == (unsigned long)NOT_SET)
        this->config.max_body_size = this->matched_serv.getMaxBdySize();
    if (this->config.max_body_size == (unsigned long)NOT_SET)
        this->config.max_body_size = this->base_config.getMaxBdySize();

    this->config.root = this->matched_loc.getRoot();
    if (this->config.root.empty())
        this->config.root = this->matched_serv.getRoot();
    if (this->config.root.empty())
        this->config.root = this->base_config.getRoot();

    // this->config.cgi_allowed_ext = this->matched_loc.getCgiAllowedExt();
    // if (this->config.cgi_allowed_ext.empty())
    //     this->config.cgi_allowed_ext = this->matched_serv.getCgiAllowedExt();
    // if (this->config.cgi_allowed_ext.empty())
    //     this->config.cgi_allowed_ext = this->base_config.getCgiAllowedExt();
    
    // this->config.cgi_path = this->matched_loc.getCgiPath();

    this->config.return_dir = this->matched_loc.getReturn();
    if (this->config.return_dir.second.empty())
        this->config.return_dir = this->matched_serv.getReturn();
    if (this->config.return_dir.second.empty())
        this->config.return_dir = this->base_config.getReturn();

    this->config.keep_alive_timeout = this->matched_loc.getKeepaliveTimeout();
    if (this->config.keep_alive_timeout == NOT_SET)
        this->config.keep_alive_timeout = this->matched_serv.getKeepaliveTimeout();
    if (this->config.keep_alive_timeout == NOT_SET)
        this->config.keep_alive_timeout = this->base_config.getKeepaliveTimeout();

    this->config.upload_dir = this->matched_loc.getUploadDir();
    if (this->config.upload_dir.empty())
        this->config.upload_dir = this->matched_serv.getUploadDir();
    if (this->config.upload_dir.empty())
        this->config.upload_dir = this->base_config.getUploadDir();
    
    this->config.cgi_extensions = this->matched_loc.getCgiExtensions();
    if (this->config.cgi_extensions.empty())
        this->config.cgi_extensions = this->matched_serv.getCgiExtensions();
    if (this->config.cgi_extensions.empty())
        this->config.cgi_extensions = this->base_config.getCgiExtensions();
    
}

void Request::init_config()
{
    // get host header (needed to find config)
    this->store_host();
    if (this->error_code)
        return ;
    
    // retrieve config
    this->fill_conf();

    this->store_body_headers(); 
    if (this->error_code)
        return ;
    
    // "allow methods": check req_line.method
    if (!this->config.allow_methods.empty() && std::find(this->config.allow_methods.begin(), this->config.allow_methods.end(), this->req_line.method) == this->config.allow_methods.end())
    {
        this->error_message = "method not allowed: " + this->req_line.method;
        this->error_code = 405;
        return ;
    }

    this->target_uri = "./" + this->config.root + this->req_line.target;
}
