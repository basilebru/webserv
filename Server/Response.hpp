#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "../Request/request_class.hpp"

class Response
{
    public:
    const Request &req;
    std::string &buf;

    public:
    Response(const Request &req, std::string &buf);
    int process();
};

#endif