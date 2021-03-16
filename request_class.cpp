#include "request_class.hpp"
#include <iostream>

Request::Request(void)
{
    // std::cout << "req created" << std::endl;
}

Request::Request(const Request &copy): req_line(copy.req_line), headers(copy.headers)
{
}

Request::~Request(void)
{
}

Request&   Request::operator=(const Request &rhs)
{
    this->req_line = rhs.req_line;
    this->headers = rhs.headers;
    return(*this);
}

