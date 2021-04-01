#ifndef REQUEST_CLASS_H
# define REQUEST_CLASS_H

#include <iostream>
#include <string>
#include <list>
#include <unistd.h> //read
#include "../Utils/get_next_line.h"
#include "../Utils/utils.hpp"

#define BUF_SIZE 1024
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
        std::string buffer;
        
        int error_code;
        std::string error_message;
        
        request_line req_line;
        std::list<header> headers;
        unsigned long body_size;
        unsigned long chunk_size;
        std::string body;
        
        bool chunked_encoding;
        bool chunked_size_read;
        bool req_line_read;
        
        // processing methods
        void check_body_headers();
        bool has_transfer_encoding();
        bool has_content_length();
        void store_body_length();
        void store_chunk_size(std::string line);
        void store_encoding();
        void store_req_line(std::string line);
        void store_header(std::string line);

        // parsing buf methods
        void parse_buffer();
        void parse_req_line();
        void parse_headers();
        void parse_body();
        void parse_body_normal();
        void parse_body_chunked();
        bool read_buf_line(std::string &line);
        bool read_chunked_size();
        bool read_chunked_data();

        // reading from socket   
        void read_from_socket();

    
    public:
        // constructor & destructor
        Request(void); // not defined
        Request(int fd);
        Request(const Request &copy); // not defined (needed ?)
        virtual ~Request(void);
        Request&  operator=(const Request &copy); // not defined

        // attributes (to be set to private...)
        bool end_of_connection;
        bool request_ready;

        // main functions
        void parse();
        void reset();

        // utils
        void print();

        // getters & setters
        int get_error_code() const;
        // void set_error_code(int code);
};

bool content_length_present(std::pair<std::string, std::string> header);
bool transfer_encoding_present(std::pair<std::string, std::string> header);

#endif