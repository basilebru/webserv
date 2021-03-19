#include "request_class.hpp"
#include <iostream>
#include <string>
#include <algorithm> // find functions
// #include <cctype>


bool is_whitespace(char c)
{
    if (c == 32 || c == 9) // space or HTAB (cf RFC 7230 3.2.3)
        return true;
    else
        return false;
}

void trim_whitespace(std::string &s)
{
    size_t pos = s.find_first_not_of("\t ");
    s.erase(0, pos);
    pos = s.find_last_not_of("\t ");
    s.erase(pos + 1);
}

int	ft_isdigit(int c)
{
	if (c >= '0' && c <= 57)
		return (1);
	else
		return (0);
}

int	ft_isdigit_str(const char *str)
{
	int i;

	if (str == 0)
		return (0);
	i = 0;
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

Request::Request(void): error_code(0), body_size(-1)
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
    // we split on whitespaces, since "no whitespace is allowed in the three components" and a single space is required between components
    if (std::count(line.begin(), line.end(), ' ') != 2)
    {
        std::cout << "parsing error: request line not well formated: " << line << std::endl;
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
    if (is_whitespace(line[0]))
    {
        std::cout << "parsing error: header line starts with whitespace: " << line << std::endl;
        this->error_code = 400;
        return ;
    }
    if ((find = line.find(':')) == std::string::npos)
    {
        std::cout << "parsing error: no double colon in header: " << line << std::endl;
        this->error_code = 400;
        return ;
    }
    field_name = line.substr(0, find);
    if (is_whitespace(field_name[field_name.size() - 1]))
    {
        std::cout << "parsing error: whitespace before double colon: " << line << std::endl;
        this->error_code = 400;
        return ;
    }
    field_value = line.substr(find + 1);
    trim_whitespace(field_value);
    this->headers.push_back(header(field_name, field_value));
}

void Request::add_body(std::string body)
{
    this->body = body;
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
    std::cout << std::endl;
    for (std::list<header>::iterator it = this->headers.begin(); it != this->headers.end(); it++)
    {
        /* code */
        std::cout << "Header line:" << std::endl;
        std::cout << " . field_name: " << "[" << it->first << "]" << std::endl;
        std::cout << " . field_value: " << "[" << it->second << "]" << std::endl;
        std::cout << std::endl;
    }
    if (this->body_size != -1)
    {
        std::cout << "body: " << this->body << std::endl;
        std::cout << std::endl;
    }
}

bool content_lenght_present(std::pair<std::string, std::string> header)
{
    if (header.first == "Content-Lenght" || header.first == "content-lenght")
        return true;
    else
        return false;
}

bool transfer_encoding_present(std::pair<std::string, std::string> header)
{
    if (header.first == "Transfer-Encoding" || header.first == "transfer-encoding")
        return true;
    else
        return false;
}

bool Request::has_transfer_encoding()
{
    if (std::find_if(this->headers.begin(), this->headers.end(), transfer_encoding_present) != this->headers.end())
        return true;
    return false;
}

bool Request::has_content_lenght()
{
    if (std::find_if(this->headers.begin(), this->headers.end(), content_lenght_present) != this->headers.end())
        return true;
    return false;
}

void Request::store_body_lenght()
{
    std::string body_lenght;
    body_lenght =  std::find_if(this->headers.begin(), this->headers.end(), content_lenght_present) ->second;
    if (ft_isdigit_str(body_lenght.c_str()) == false)
    {
        std::cout << "parsing error: Content-Lenght header value is invalid: " << body_lenght << std::endl;
        this->error_code = 400;
        return ;
    }
    if (body_lenght.length() > 7) // > 9.9999 MB
    {
        std::cout << "parsing error: Content-Lenght header value is too big: " << body_lenght << std::endl;
        this->error_code = 400;
        return ;
    }
    int ret(0);
    int i(0);
	while (body_lenght[i])
	{
		ret = ret * 10 + body_lenght[i] - 48;
		i++;
	}
    this->body_size = ret;
    // std::cout << "body lenght is: " << this->body_size << std::endl;
}

void Request::parse_body_headers()
{
    // if (this->has_transfer_encoding())
    //     std::cout << "transfer-encoding header detected" << std::endl;
    if (this->error_code)
        return ;

    if (this->has_content_lenght())
    {
        // std::cout << "content-lenght header detected" << std::endl;
        this->store_body_lenght();
    }
}

int Request::get_error_code() const
{
    return this->error_code;
}

int Request::get_body_lenght() const
{
    return this->body_size;
}