#ifndef REQUEST_CLASS_H
# define REQUEST_CLASS_H

#include <iostream>
#include <string>
#include <list>


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

        int error_code;
        request_line req_line;
        std::list<header> headers;
    public:
        Request(void);
        Request(const Request &copy);
        virtual ~Request(void);
        Request&  operator=(const Request &copy);

        void add_req_line(std::string line);
        void add_header(std::string line);
        void print();
        bool body_expected();
        // void add_body
};

bool is_whitespace(char c);
void trim_whitespace(std::string &s);

#endif