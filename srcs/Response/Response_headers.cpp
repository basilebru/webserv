# include "Response.hpp"

void Response::build_headers()
{
    std::cout << "building headers" << std::endl;
    
    this->build_content_length();
    this->build_content_type();
    this->build_keep_alive();
    this->build_location_if_redirection();

    this->headers += "\r\n";
    
    this->response.insert(this->response.begin(), this->headers.begin(), this->headers.end());
    
    this->build_response_line();
	
    std::cout << "Contenu de la reponse:" << std::endl;
    std::cout << "---------------------------" << std::endl;
    for (size_t i = 0; i < this->response.size(); ++i)
    {
        std::cout << this->response[i];
    }
    std::cout << std::endl;
    std::cout << "---------------------------" << std::endl;
}

void Response::build_content_length()
{
    this->headers += "Content-length: ";
    this->headers += iToString(this->response.size());
    this->headers += "\r\n";
}

void Response::build_content_type()
{
    this->headers += "Content-type: ";
    
    bool has_known_extension;
    str_map::iterator it;
    it = this->extension_map.find(this->extension); 
    has_known_extension = it != this->extension_map.end();
    if (has_known_extension)
        this->headers += it->second;
    else
        this->headers += "application/octet-stream"; // ou text/plain ?
    
    this->headers += "\r\n";

}
void Response::build_location_if_redirection()
{
    if (this->redir_activated)
    {
        std::cout << "Redirection activated" << std::endl;
        this->headers += "Location: ";
        this->headers += this->redir_target;
        this->headers += "\r\n";
    }
}

void Response::build_keep_alive()
{
    bool keep_alive_timeout_set = this->req.config.keep_alive_timeout != NOT_SET;
    if (keep_alive_timeout_set)
    {
        this->headers += "Connection: ";
        this->headers += "Keep-alive";
        this->headers += "\r\n";
        this->headers += "Keep-alive: timeout=";
        this->headers += iToString(this->req.config.keep_alive_timeout);
        this->headers += "\r\n";
    }
    // OR
    // this->headers += "Connection: ";
    // this->headers += "Close";
    // this->headers += "\r\n";
}

void Response::build_response_line()
{
    std::string ret;
    ret = "HTTP/1.1 ";
    ret += iToString(this->response_code);
    ret += " ";
    ret += code_map[this->response_code];
    ret += "\r\n";
    this->response.insert(this->response.begin(), ret.begin(), ret.end());
}


