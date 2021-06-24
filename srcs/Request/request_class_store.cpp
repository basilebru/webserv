#include "request_class.hpp"

void Request::store_req_line(std::string line)
{
    size_t count(0);
    size_t pos(0);
    size_t next;
    // we split on whitespaces, since "no whitespace is allowed in the three components" and a single space is required between components
    if (std::count(line.begin(), line.end(), ' ') != 2)
    {
        this->error_message = "parsing error: request line not well formated: " + line;
        this->error_code = 400;
        return ;
    }
    while((next = line.find(" ", pos)) != std::string::npos)
    {
        if (count == 0)
            this->req_line.method = line.substr(pos, next - pos);
        else // count == 1
            this->req_line.target = line.substr(pos, next - pos);
        pos = next + 1;
        count++;
    }
    this->req_line.version = line.substr(pos);

    // separate query_string from target
    pos = this->req_line.target.find('?');
    if (pos != std::string::npos)
    {
        this->req_line.query_string = this->req_line.target.substr(pos + 1);
        this->req_line.target.erase(pos);
    }
    
    // fill extension
    pos = this->req_line.target.find('.');
    if (pos != std::string::npos)
        this->req_line.extension = this->req_line.target.substr(pos + 1);
    
    
    // check that method is known
    if (std::find(Request::known_methods.begin(), Request::known_methods.end(), this->req_line.method) == Request::known_methods.end())
    {
        this->error_message = "method not implemented: " + this->req_line.method;
        this->error_code = 501;
        return ;
    }
    // check that HTTP version is 1.1
    if (this->req_line.version != "HTTP/1.1")
    {
        this->error_message = "version not supported: " + this->req_line.version;
        this->error_code = 505;
    }
    delete_consecutive_slashes_in_uri(this->req_line.target);
    return;
}

void Request::store_header(std::string line)
{
    size_t find;
    std::string field_name;
    std::string field_value;
    
    if (is_whitespace(line[0]))
    {
        this->error_message = "parsing error: header line starts with whitespace: " + line;
        this->error_code = 400;
        return ;
    }
    if ((find = line.find(':')) == std::string::npos)
    {
        this->error_message = "parsing error: no double colon in header: " + line;
        this->error_code = 400;
        return ;
    }
    field_name = line.substr(0, find);
    if (is_whitespace(field_name[field_name.size() - 1]))
    {
        this->error_message = "parsing error: whitespace before double colon: " + line;
        this->error_code = 400;
        return ;
    }
    field_value = line.substr(find + 1);
    trim_whitespace(field_value);
    this->headers[to_lower(field_name)] = field_value;
}


void Request::store_host()
{
    // check that there is a host header
    // store host field valeu
    if (!this->has_host())
    {
        this->error_message = "parsing error: no host header";
        this->error_code = 400;
        return ;
    }
    std::string host;
    std::string port;
    host = this->headers["host"];
    size_t find;
    if ((find = host.find(':')) == std::string::npos)
    {
        this->host_uri = host;
        return;
    }
    else
    {
        this->host_uri = host.substr(0, find);
        this->host_port = host.substr(find + 1);
    }
    // if (!port.empty() && ft_isdigit_str(port.c_str()))
    //     this->host_port = strtol(port.c_str(), NULL,10);
    // else
    // {
    //     this->error_message = "parsing error: port number format";
    //     this->error_code = 400;
    // }
}

void Request::store_body_headers()
{
    if (this->headers.count("transfer-encoding"))
    {
        this->store_encoding();
        this->body_size = 0;
        return ; // priorité au transfer-encoding header sur le body-length header
    }
    if (this->headers.count("content-length"))
        this->store_body_size();
}


void Request::store_chunk_size(std::string line)
{
    if (ft_isxdigit_str(line.c_str()) == false)
    {
        this->error_message = "parsing error: chunk size value is invalid: " + line;
        this->error_code = 400;
        return ;
    }
    if (line.length() > 6) // > FFFFFF = 1,049 MB
    {
        this->error_message = "parsing error: chunk-size value is too big: " + line;
        this->error_code = 400;
        return ;
    }
    this->chunk_size = strtol(line.c_str(), NULL, 16);
    this->body_size += this->chunk_size;
    std::cout << "body size: " << this->body_size << std::endl;
    if (this->config.max_body_size != NOT_SET && this->body_size > (unsigned long)this->config.max_body_size)
    {
        this->error_message = "body size > max_body_size";
        this->error_code = 413;
    }
}

void Request::store_encoding()
{
    std::string encoding;
    encoding =  this->headers["transfer-encoding"];
    if (to_lower(encoding) == "chunked")
        this->chunked_encoding = true;
    else
    {
        this->error_message = "parsing error: transfer-encoding name not supported: " + encoding;
        this->error_code = 500;
    }
}

void Request::store_body_size()
{
    std::string body_size;
    body_size =  this->headers["content-length"];
    if (ft_isdigit_str(body_size.c_str()) == false)
    {
        this->error_message = "parsing error: Content-length header value is invalid: " + body_size;
        this->error_code = 400;
        return ;
    }
    if (body_size.length() > 7) // > 9.9999 MB
    {
        this->error_message = "parsing error: Content-length header value is too big: " + body_size;
        this->error_code = 400;
        return ;
    }
    // int ret(0);
    // int i(0);
	// while (body_size[i])
	// {
	// 	ret = ret * 10 + body_size[i] - 48;
	// 	i++;
	// }
    // this->body_size = ret;
    this->body_size = strtol(body_size.c_str(), NULL, 10);
    if (this->config.max_body_size != NOT_SET && this->body_size > (unsigned long)this->config.max_body_size)
    {
        this->error_message = "body size > max_body_size";
        this->error_code = 413;
    }
}

bool Request::has_transfer_encoding()  const
{
    if (std::find_if(this->headers.begin(), this->headers.end(), transfer_encoding_present) != this->headers.end())
        return true;
    return false;
}

bool Request::has_content_length() const
{
    if (std::find_if(this->headers.begin(), this->headers.end(), content_length_present) != this->headers.end())
        return true;
    return false;
}

bool Request::has_host() const
{
    if (std::find_if(this->headers.begin(), this->headers.end(), host_present) != this->headers.end())
        return true;
    return false;
}

bool transfer_encoding_present(std::pair<std::string, std::string> header)
{
    if (header.first == "Transfer-Encoding" || header.first == "transfer-encoding")
        return true;
    else
        return false;
}

bool host_present(std::pair<std::string, std::string> header)
{
    if (header.first == "Host" || header.first == "host")
        return true;
    else
        return false;
}

bool content_length_present(std::pair<std::string, std::string> header)
{
    if (header.first == "Content-Length" || header.first == "content-length")
        return true;
    else
        return false;
}

