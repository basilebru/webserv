#include "Response.hpp"

Response::Response(const Request &req): req(req)
{
}

int Response::process()
{
    if (this->req.connection_end() || this->req.get_error_code())
    {
        // log message
        if (this->req.connection_end())
            std::cout << RED << "Client closed connection" << NOCOLOR << std::endl;
        else
            std::cout << RED << "Request error, closing connection" << NOCOLOR << std::endl;
        return -1;
    }
    if (this->req.request_is_ready())
    {
        std::cout << "Request ready to be treated" << std::endl;
        this->buf = "lol";
        std::cout << ".............." << std::endl;
        std::cout << "Request deleted" << std::endl;
        return 1;
    }
    return 0;
}
