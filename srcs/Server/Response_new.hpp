#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "CgiHandler.hpp"
# include "Autoindex.hpp"
# include "request_class.hpp"

class Response {

private:
    const Request               &req;
    std::string                 buf;
    std::vector<unsigned char>  &response;
    std::vector<unsigned char>  body;
    std::vector<unsigned char>  headers;
    void build_response();
    void index_module();
    void get_file_module();
    void exec_cgi(std::string const& path);
    void send_img(std::string const& path);
    
    Response(void);

public:
    Response(const Request &req, std::vector<unsigned char> &buf);
    Response(Response const & copy);
    ~Response(void);
    Response& operator=(Response const & rhs);

    int process();
};

std::ostream & operator<<(std::ostream & o, Response const & rhs);

#endif // RESPONSE_HPP
