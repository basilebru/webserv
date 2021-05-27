#include "Response.hpp"

Response::Response(const Request &req, std::string &buf): req(req), buf(buf)
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
        this->buf = "here is my response :)\n";
        // test big buffer (multiple select calls)
        // this->buf.assign(9000000, 'a');
        // this->buf.push_back('\n');
        std::cout << ".............." << std::endl;
        std::cout << "Request deleted" << std::endl;
        return 1;
    }
    return 0;
}
