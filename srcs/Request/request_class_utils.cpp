#include "request_class.hpp"

std::vector<std::string> Request::build_known_methods()
{
    std::vector<std::string> methods;
    methods.push_back("GET");
    methods.push_back("HEAD");
    methods.push_back("POST");
    methods.push_back("PUT");
    methods.push_back("DELETE");
    methods.push_back("OPTIONS");
    methods.push_back("TRACE");
    // methods.push_back("CONNECT");
    // methods.push_back("PATCH");
    return methods;
}

void                    Request::initHeaders(void)
{
    this->headers["accept-charsets"] = "";
    this->headers["accept-language"] = "";
    this->headers["allow"] = "";
    this->headers["auth-scheme"] = "";
    this->headers["authorization"] = "";
    this->headers["content-language"] = "";
    this->headers["content-length"] = "";
    this->headers["content-location"] = "";
    this->headers["content-type"] = "";
    this->headers["date"] = "";
    this->headers["host"] = "";
    this->headers["last-modified"] = "";
    this->headers["location"] = "";
    this->headers["referer"] = "";
    this->headers["retry-after"] = "";
    this->headers["server"] = "";
    this->headers["transfer-encoding"] = "";
    this->headers["user-agent"] = "";
    this->headers["www-authenticate"] = "";
    this->headers["connection"] = "";
}

std::vector<std::string> Request::known_methods = Request::build_known_methods();


// // reset is called before each request, to "clear" the previous request and init attributes (it is also called for the first request)
// void Request::reset()
// {
//     std::cout << "reseting request" << std::endl;
//     // set bools to false
//     this->chunked_encoding = false;
//     this->req_line_read = false;
//     this->request_ready = false;
//     this->chunked_size_read = false;
//     // reset req line, headers & body
//     this->req_line.method = "";
//     this->req_line.version = "";
//     this->req_line.target = "";
//     this->headers.erase(this->headers.begin(), this->headers.end());
//     this->body = "";
//     this->body_size = 0;
//     this->chunk_size = 0;
//     // this->host_port = 0;
//     this->host_uri = "";
//     // config
//     this->config.root = "";
//     this->config.max_body_size = NOT_SET;
//     this->config.autoindex = NOT_SET;
//     this->config.index.erase(this->config.index.begin(), this->config.index.end());
//     this->config.allow_methods.erase(this->config.allow_methods.begin(), this->config.allow_methods.end());
//     this->config.error_pages.erase(this->config.error_pages.begin(), this->config.error_pages.end());

//     // reset serverblock and location block
// }

int Request::get_error_code() const
{
    return this->error_code;
}

int Request::get_fd() const
{
    return this->fd;
}

sockaddr_in Request::get_addr() const
{
    return this->address;
}

bool Request::connection_end() const
{
    return this->end_of_connection;
}

bool Request::request_is_ready() const
{
    return this->request_ready;
}

std::map<std::string, std::string> const& Request::get_headers() const
{
    return this->headers;
} 

std::map<int, std::string> const&     Request::getErrorPages(void) const
{
    return this->config.error_pages;
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
    for (std::map<std::string, std::string>::const_iterator it = this->headers.begin(); it != this->headers.end(); it++)
    {
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

void    Request::print2(void) const
{
    std::cout << "REQUEST DETAILS: " << std::endl;
    std::cout << "--------------------------------" << std::endl;

    std::cout << "HOST: " << this->host_uri << std::endl;
    std::cout << "METHOD: " << this->req_line.method << std::endl;
    std::cout << "TARGET: " << this->req_line.target << std::endl;
    std::cout << "VERSION: " << this->req_line.version << std::endl;
    std::cout << "TARGET PATH: " << this->target_uri << std::endl;
    
    std::cout << "BODY: " << this->body << std::endl;
    std::cout << "BDY SIZE: " << iToString(this->body_size) << std::endl;
    
    std::cout << "CHUNK SIZE: " << iToString(this->chunk_size) << std::endl;

    std::cout << "--------------------------------" << std::endl;
}
