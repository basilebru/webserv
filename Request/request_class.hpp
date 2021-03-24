#ifndef REQUEST_CLASS_H
# define REQUEST_CLASS_H

#include <iostream>
#include <string>
#include <list>
#include <unistd.h> //read
#include "../Utils/get_next_line.h"
#include "../Utils/utils.hpp"


struct request_line
{
    std::string method;
    std::string target;
    std::string version;
};

class Request
{
    private:
        typedef std::pair<std::string, std::string> header;

        // attributes
        int fd;
        int error_code;
        request_line req_line;
        std::list<header> headers;
        int body_size;
        std::string body;
        bool chunked_encoding;
        
        // private functions -- body parsing
        void parse_body_headers();
        bool has_transfer_encoding();
        bool has_content_length();
        void store_body_length();
        void store_encoding();
        void read_chunked(int connection);
        void read_normal(int connection);
        int readline(std::string &line);
    
    public:
        // constructor & destructor
        Request(void);
        Request(int fd);
        Request(const Request &copy);
        virtual ~Request(void);
        Request&  operator=(const Request &copy);

        // main functions
        void add_req_line(std::string line);
        void add_header(std::string line);
        void parse_body(int connection);
        int parse();

        // utils
        void print();

        // getters & setters
        int get_error_code() const;
        void set_error_code(int code);
};

int crlf_gnl(int connection, std::string &line, Request &req);

#endif