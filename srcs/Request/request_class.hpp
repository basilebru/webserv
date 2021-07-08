#ifndef REQUEST_CLASS_H
# define REQUEST_CLASS_H

# include "ConfParser.hpp"

# define BUF_SIZE 65536
# define URI_MAX_LEN 100000
# define HEADER_MAX_LEN 100000

struct request_line
{
    std::string method;
    std::string target;
    std::string extension;
    std::string query_string;
    std::string version;
};

struct conf
{
        unsigned long               max_body_size;
        int                         keep_alive_timeout;
        std::vector<std::string>    allow_methods;
        std::string                 root;
        std::vector<std::string>    index;
        int                         autoindex;
        errorMap                    error_pages;
        std::pair<int, std::string> return_dir;
        std::string                 cgi_path;
        std::string                 upload_dir;
        stringMap                   cgi_extensions;
};

class Request
{
    friend class Response;
    friend class CgiHandler;
    friend class Autoindex;
    
    private:
        // known HTTP methods
        static std::vector<std::string> known_methods;
        static std::vector<std::string> build_known_methods();
        // ctrl_c string
        static std::vector<unsigned char> ctrl_c;
        static std::vector<unsigned char> build_ctrl_c();


        // reading from connection
        int fd;
        std::vector<unsigned char> buffer;
        
        // error
        int error_code;
        std::string error_message; // for debug purpose
        
        // storing request
        request_line req_line;
        std::map<std::string, std::string> headers;
        unsigned long body_size;
        unsigned long chunk_size;
        std::vector<unsigned char> body;
        std::string host_uri;
        std::string host_port;
        
        // bools
        bool chunked_encoding;
        bool chunked_size_read;
        bool req_line_read;
        bool end_of_connection;
        bool request_ready;

        // config
        conf config;
        ServerBlock matched_serv;
        LocationBlock matched_loc;
        sockaddr_in address; // ip and port of the server connected to clientwho sent the request in order to find suitable server_block
        const std::vector<ServerBlock> &servers;
        const HttpBlock &base_config;

        // METHODS

        // "storing" methods
        void store_req_line(std::string line);
        void store_header(std::string line);
        void store_body_headers();
        void store_body_size();
        void store_chunk_size(std::string line);
        void store_encoding();
        void store_host();
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
        std::vector<unsigned char>::iterator find_crlf();
        bool read_buf_line(std::string &line);
        void read_from_socket();

        // config
        void init_config(); //Called in Request::parse_headers()
        void fill_conf();
        void match_server();
        void match_location();
        int match_wildcard_locations();

    
    public:
        // constructor & destructor
        Request(void); // not defined
        Request(int fd, sockaddr_in addr, const std::vector<ServerBlock> &servers, const HttpBlock &base_config);
        Request(const Request &copy); // not defined (needed ?)
        virtual ~Request(void);
        Request&  operator=(const Request &copy); // not defined

        // main functions
        void parse();
        // utils
        void print(void) const;
        void print_buffer() const;
        void print_config() const;
        void print2(void) const;
        // getters & setters
        int get_error_code() const;
        int get_fd() const;
        sockaddr_in get_addr() const;
        bool request_is_ready() const;
        std::map<std::string, std::string> const& get_headers(void) const;
        std::map<int, std::string> const&     getErrorPages(void) const;
        std::map<std::string, std::string> const& getCgi_extensions(void) const;
        bool connection_end() const;
        // void set_error_code(int code);

};

void delete_consecutive_slashes_in_uri(std::string &uri);

#endif