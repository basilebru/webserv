#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "../Request/request_class.hpp"

class Response
{
    private:
    const Request &req;
    std::string &buf;
    void build_response();

    public:
    Response(const Request &req, std::string &buf);
    int process();
};

#endif