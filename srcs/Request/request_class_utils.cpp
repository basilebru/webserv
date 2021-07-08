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

// void                    Request::initHeaders(void)
// {
//     this->headers["accept-charsets"] = "";
//     this->headers["accept-language"] = "";
//     this->headers["allow"] = "";
//     this->headers["auth-scheme"] = "";
//     this->headers["authorization"] = "";
//     this->headers["content-language"] = "";
//     this->headers["content-length"] = "";
//     this->headers["content-location"] = "";
//     this->headers["content-type"] = "";
//     this->headers["date"] = "";
//     this->headers["host"] = "";
//     this->headers["last-modified"] = "";
//     this->headers["location"] = "";
//     this->headers["referer"] = "";
//     this->headers["retry-after"] = "";
//     this->headers["server"] = "";
//     this->headers["transfer-encoding"] = "";
//     this->headers["user-agent"] = "";
//     this->headers["www-authenticate"] = "";
//     this->headers["connection"] = "";
// }

std::vector<std::string> Request::known_methods = Request::build_known_methods();

std::vector<unsigned char> Request::ctrl_c = Request::build_ctrl_c();

std::vector<unsigned char> Request::build_ctrl_c()
{
    std::vector<unsigned char> vec;
    vec.push_back('\xFF');
    vec.push_back('\xF4');
    vec.push_back('\xFF');
    vec.push_back('\xFD');
    vec.push_back('\x06');
    return vec;
        // if (ret == 5 && strcmp(buf, "\xFF\xF4\xFF\xFD\x06") == 0)
}


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

std::map<std::string, std::string> const& Request::get_headers(void) const
{
    return this->headers;
} 

std::map<int, std::string> const&     Request::getErrorPages(void) const
{
    return this->config.error_pages;
}

std::map<std::string, std::string> const& Request::getCgi_extensions(void) const
{
    return this->config.cgi_extensions;
} 

// void Request::set_error_code(int code)
// {
//     this->error_code = code;
// };

void Request::print() const
{
    std::cout << "REQUEST DETAILS: " << std::endl;
    std::cout << "--------------------------------" << std::endl;
    if (this->error_code)
    {
        std::cout << "Error:" << error_code << std::endl;
        std::cout << error_message << std::endl;
        return ;
    }
    std::cout << "REQUEST LINE:";
    std::cout << " " << this->req_line.method;
    std::cout << " " << this->req_line.target;
    std::cout << " " << this->req_line.version << std::endl;
    std::cout << "HEADERS:" << std::endl;
    for (std::map<std::string, std::string>::const_iterator it = this->headers.begin(); it != this->headers.end(); it++)
    {
        std::cout << " . " << "[" << it->first << "]";
        std::cout << " " << "[" << it->second << "]" << std::endl;
    }
    std::cout << "BDY SIZE: " << iToString(this->body_size) << std::endl;
    // if (!this->body.empty())
    // {
    //     std::string bdy;
    //     bdy.assign(this->body.begin(), this->body.end());
    //     std::cout << "body: " << bdy << std::endl;
    //     std::cout << std::endl;
    // }
    std::cout << "--------------------------------" << std::endl;
}

void    Request::print2(void) const
{
    std::cout << "REQUEST DETAILS: " << std::endl;
    std::cout << "--------------------------------" << std::endl;

    std::cout << "HOST: " << this->host_uri << std::endl;
    std::cout << "PORT: " << this->host_port << std::endl;
    std::cout << "METHOD: " << this->req_line.method << std::endl;
    std::cout << "TARGET: " << this->req_line.target << std::endl;
    std::cout << "VERSION: " << this->req_line.version << std::endl;
    std::cout << "TARGET PATH: " << this->target_uri << std::endl;
    
    std::string bdy;
    bdy.assign(this->body.begin(), this->body.end());
    std::cout << "BODY: " << bdy << std::endl;
    std::cout << "BDY SIZE: " << iToString(this->body_size) << std::endl;
    
    std::cout << "CHUNK SIZE: " << iToString(this->chunk_size) << std::endl;

    std::cout << "--------------------------------" << std::endl;
}

void Request::print_buffer() const
{
    std::string buf;
    buf.assign(this->buffer.begin(), this->buffer.end());
    std::cout << "buffer: " << buf << std::endl;
    std::cout << "buffer-size:" << buf.size() << std::endl;
}

void Request::print_config() const
{
    std::cout << "CONFIG DETAILS:" << std::endl;
    std::cout << "--------------------------------" << std::endl;
    std::cout <<"MATCHED_SERVER_NAME: " << this->matched_serv.getServerNames()[0] << std::endl;
    std::cout << "MATCHED LOC PATH: " << this->matched_loc.getPath() << std::endl;
    std::cout << "MAX_BDY_SIZE: " << this->config.max_body_size << std::endl;
    std::cout << "KA TIMEOUT: " << this->config.keep_alive_timeout << std::endl;
    std::cout << "ALLOW METHODS: " ;
    putVecToOstream(std::cout, this->config.allow_methods.begin(), this->config.allow_methods.end());
    std::cout << "ROOT: " << this->config.root << std::endl;
    std::cout << "INDEX: " ;
    putVecToOstream(std::cout, this->config.index.begin(), this->config.index.end());
    std::cout << "AUTOINDEX: " << this->config.autoindex << std::endl;
    std::cout << "ERROR_PAGES: " ;
    putMapToOstream(std::cout, this->config.error_pages.begin(), this->config.error_pages.end());
    std::cout << "RETURN DIR: " << this->config.return_dir.first << " -- " << this->config.return_dir.second << std::endl;
    std::cout << "CGI_EXTENSIONS: " ;
    putMapToOstream(std::cout, this->config.cgi_extensions.begin(), this->config.cgi_extensions.end());
    // std::cout << "CGI PATH: " << this->config.cgi_path << std::endl;
    std::cout << "--------------------------------" << std::endl;
}
