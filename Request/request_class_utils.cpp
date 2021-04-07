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

std::vector<std::string> Request::known_methods = Request::build_known_methods();

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
    // this->host_port = 0;
    this->host_uri = "";
    // config
    this->root = "";
    this->max_body_size = 0;
    // this->index.erase(this->index.begin(), this->index.end());
    this->allow_methods.erase(this->allow_methods.begin(), this->allow_methods.end());
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
