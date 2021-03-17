#include "request_class.hpp"
#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>

Request::Request(void): error_code(0)
{
}

Request::Request(const Request &copy): req_line(copy.req_line), headers(copy.headers)
{
}

Request::~Request(void)
{
}

Request&   Request::operator=(const Request &rhs)
{
    this->req_line = rhs.req_line;
    this->headers = rhs.headers;
    return(*this);
}

void Request::add_req_line(std::string line)
{
    size_t count(0);
    size_t pos(0);
    size_t next;

    if (std::count(line.begin(), line.end(), ' ') != 2)
    {
        this->error_code = 400;
        return ;
    }
    while((next = line.find(" ", pos)) != std::string::npos)
    {
        //std::cout << "pos: " << pos << std::endl;
        //std::cout << "next: " << next << std::endl;
        // std::cout << "sub: " << line.substr(pos, next - pos) << std::endl;
        if (count == 0)
            this->req_line.method = line.substr(pos, next - pos);
        else // count == 1
            this->req_line.target = line.substr(pos, next - pos);
        pos = next + 1;
        count++;
    }
    this->req_line.version = line.substr(pos);
}

void Request::add_header(std::string line)
{
    size_t find;
    std::string field_name;
    std::string field_value;
    if ((find = line.find(':')) == std::string::npos)
    {
        this->error_code = 400;
        return ;
    }
    field_name = line.substr(0, find);
    field_value = line.substr(find + 1);
    if (std::isspace(field_name.back()))
    {
        this->error_code = 400;
        return ;
    }
    // trim SP and HT in field_value
    this->headers.push_back(std::pair<std::string, std::string>(field_name, field_value));
}

void Request::print()
{
    if (this->error_code)
    {
        std::cout << "Error:" << error_code << std::endl;
        return ;
    }
    std::cout << "Request line:" << std::endl;
    std::cout << " . Method: " << this->req_line.method << std::endl;
    std::cout << " . Target: " << this->req_line.target << std::endl;
    std::cout << " . Version: " << this->req_line.version << std::endl;
}

