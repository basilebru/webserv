
// Class that doesnt use GNL to read(): use of an internal buffer to store successive reads (in case request doesnt arrive line by line)
// : use of an internal buffer to store u (in case request doesnt arrive line by linecould rece)TO DO: read_chunked


#ifndef REQUEST_CLASS_H
# define REQUEST_CLASS_H

#include <iostream>
#include <string>
#include <list>
#include <unistd.h> //read
#include "../Utils/get_next_line.h"
#include "../Utils/utils.hpp"

#define BUF_SIZE 256
#define SUCCESS 1


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
        std::string buffer;
        
        // processing
        void check_body_headers();
        bool has_transfer_encoding();
        bool has_content_length();
        void store_body_length();
        void store_encoding();
        void store_req_line(std::string line);
        void store_header(std::string line);

        // reading
        int readline(std::string &line);
        void read_chunked();
        int read_normal();
        
        // main
        void parse_req_line();
        void parse_headers();
        int parse_body();
        void parse_buffer();
        void cut_buf_line(std::string &line);

    
    public:
        // constructor & destructor
        Request(void);
        Request(int fd);
        Request(const Request &copy);
        virtual ~Request(void);
        Request&  operator=(const Request &copy);

        // main functions
        int parse();

        // utils
        void print();

        // getters & setters
        int get_error_code() const;
        // void set_error_code(int code);
};

bool content_length_present(std::pair<std::string, std::string> header);
bool transfer_encoding_present(std::pair<std::string, std::string> header);

#endif