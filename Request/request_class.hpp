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
        std::string error_message;
        request_line req_line;
        std::list<header> headers;
        int body_size;
        std::string body;
        bool chunked_encoding;
        
        // processing
        void check_body_headers();
        bool has_transfer_encoding();
        bool has_content_length();
        void store_body_length();
        void store_encoding();
        void store_req_line(std::string line);
        void store_header(std::string line);

        // reading
        void readline(std::string &line);
        void set_read_error(int ret);
        void read_chunked();
        void read_normal();
        
        // main
        void parse_req_line();
        void parse_headers();
        void parse_body();

    
    public:
        // constructor & destructor
        Request(void);
        Request(int fd);
        Request(const Request &copy);
        virtual ~Request(void);
        Request&  operator=(const Request &copy);

        // main functions
        void parse();

        // utils
        void print();

        // getters & setters
        int get_error_code() const;
        // void set_error_code(int code);
};

bool content_length_present(std::pair<std::string, std::string> header);
bool transfer_encoding_present(std::pair<std::string, std::string> header);

#endif