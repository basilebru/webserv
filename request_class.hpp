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
        request_line req_line;
        std::list<std::pair<std::string, std::string> > headers;
    public:
        Request(void);
        Request(const Request &copy);
        virtual ~Request(void);
        Request&  operator=(const Request &copy);
};

#endif