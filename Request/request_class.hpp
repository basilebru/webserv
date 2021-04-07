#ifndef REQUEST_CLASS_H
# define REQUEST_CLASS_H

#include <iostream>
#include <string>
#include <list>
#include <unistd.h> //read
#include "../Utils/get_next_line.h"
#include "../Utils/utils.hpp"
#include <vector>

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

        static std::vector<std::string> known_methods;
        static std::vector<std::string> build_known_methods();

        // reading from connection
        int fd;
        std::string buffer;
        
        // error
        int error_code;
        std::string error_message; // for debug purpose
        
        // storing request
        request_line req_line;
        std::list<header> headers;
        unsigned long body_size;
        unsigned long chunk_size;
        std::string body;
        std::string host_uri;
        // int host_port;
        
        // bools
        bool chunked_encoding;
        bool chunked_size_read;
        bool req_line_read;
        bool end_of_connection;
        bool request_ready;

        // config
        size_t max_body_size;
        std::string root;
        std::vector<std::string> index;
        
        // config (experimental)
        void init_config(); // just to try using some config params

        // "storing" methods
        void store_req_line(std::string line);
        void store_header(std::string line);
        void store_body_headers();
        void store_body_size();
        void store_chunk_size(std::string line);
        void store_encoding();
        void store_host();

        // utils
        bool read_buf_line(std::string &line);
        bool has_transfer_encoding() const;
        bool has_content_length() const;
        bool has_host() const;

        // main parsing methods
        void parse_buffer();
        void parse_req_line();
        void parse_headers();
        void parse_body();
        void parse_body_normal();
        void parse_body_chunked();
        bool parse_chunked_size();
        bool parse_chunked_data();

        // reading from socket   
        void read_from_socket();


    
    public:
        // constructor & destructor
        Request(void); // not defined
        Request(int fd);
        Request(const Request &copy); // not defined (needed ?)
        virtual ~Request(void);
        Request&  operator=(const Request &copy); // not defined


        // main functions
        void parse();
        void reset();

        // utils
        void print() const;

        // getters & setters
        int get_error_code() const;
        bool request_is_ready() const;
        bool connection_end() const;
        // void set_error_code(int code);
        
};

bool content_length_present(std::pair<std::string, std::string> header);
bool transfer_encoding_present(std::pair<std::string, std::string> header);
bool host_present(std::pair<std::string, std::string> header);


#endif