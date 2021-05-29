#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "request_class.hpp"
# include "CgiHandler.hpp"

class Response
{
    private:
    const Request &req;
    std::string &buf;
    void build_response();
    void exec_cgi(void);


    public:
    Response(const Request &req, std::string &buf);
    int process();
};

#endif