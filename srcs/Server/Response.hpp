#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "request_class.hpp"
# include "CgiHandler.hpp"
# include "Autoindex.hpp"

class Response
{
    private:
    const Request               &req;
    std::string                 buf;
    std::vector<unsigned char>  &response;
    
    void build_response();
    void exec_cgi(std::string const& path);
    void send_img(std::string const& path);


    public:
    Response(const Request &req, std::vector<unsigned char> &buf);
    ~Response(void);
    int process();
};

#endif