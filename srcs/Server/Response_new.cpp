# include "Response_new.hpp"
# include "Autoindex.hpp"
# include "CgiHandler.hpp"
# include <iterator>

Response::Response(const Request &req, std::vector<unsigned char> &buf): req(req), response(buf)
{
    this->extension = this->req.req_line.extension;
    this->response_code = 0;
    // std::cout << "Response created" << std::endl;
}

Response::~Response(void)
{
    // std::cerr << "RESPONSE DESTRUCTOR" << std::endl;
}

// returns CLOSE if connection needs to be closed (error during request or EOF received)
// returns SKIP if request is not ready to be processed (not entirely received)
// returns NEW if request has been processed, buffer is filled -> request can be deleted
int Response::process()
{
    if (this->req.connection_end()) // client has disconected (read received EOF)
    {
        std::cout << RED << "Client closed connection" << NOCOLOR << std::endl;
        return CLOSE;
    }
    if (this->req.error_code) // request not well formated, ctrl-c... (like nginx: closes connection)
    {
        this->error_module(this->req.error_code);
        return CLOSE;
    }
    if (this->req.request_is_ready())
    {
        std::cout << "Request ready to be treated" << std::endl;
        this->req.print_config();
        try
        {
            this->build_response();
        }
        catch(const std::exception& e)
        {
            std::cerr << "exception caught: " << e.what() << std::endl;
            this->error_module(500);
            return CLOSE;
        }
        return NEW;
    }
    return SKIP;
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

bool my_equal(char c1, char c2)
{
    if (toupper(c1) == toupper(c2))
        return true;
    return false;
}

Response::str_map Response::extension_map = Response::init_ext_map();

Response::str_map Response::init_ext_map()
{
    str_map mp;
    mp["html"] = "text/html";
    mp["txt"] = "text/plain";
    mp["css"] = "text/css";
    mp["js"] = "text/javascript";
    mp["jpeg"] = "image/jpeg";
    mp["jpg"] = "image/jpeg";
    mp["png"] = "image/png";
    mp["bmp"] = "image/bmp";
    mp["gif"] = "image/gif";
    mp["ico"] = "image/x-icon";
    return mp;
}

Response::int_map Response::code_map = Response::init_code_map();

Response::int_map Response::init_code_map()
{
    int_map mp;
    mp[200] = "OK";
    mp[201] = "Created";
    mp[301] = "Moved permanently";
    mp[400] = "Bad Request";
    mp[403] = "Forbidden";
    mp[404] = "Not found";
    mp[405] = "Method not allowed";
    mp[413] = "Request entity too large";
    mp[500] = "Internal server error";
    mp[501] = "Not implemented";
    mp[505] = "HTTP version not supported";
    return mp;
}

std::string Response::delete_response = "<html>\
<body>\
<h1>File deleted.</h1>\
</body>\
</html>";


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
    bool redirection_activated = !this->req.config.return_dir.second.empty();
    if (redirection_activated)
    {
        this->headers += "Location: ";
        this->headers += this->req.config.return_dir.second;
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
	
    // std::cout << "Contenu de la reponse:" << std::endl;
    // std::cout << "---------------------------" << std::endl;
    // for (size_t i = 0; i < this->response.size(); ++i)
    // {
    //     std::cout << this->response[i];
    // }
    // std::cout << std::endl;
    // std::cout << "---------------------------" << std::endl;
}

void Response::get_module()
{
    bool loc_has_return_directive = !this->req.config.return_dir.second.empty();
    if (loc_has_return_directive == true)
    {
        this->response_code = this->req.config.return_dir.first;
        return this->build_headers();
    }

    bool target_ends_with_slash = this->target[this->target.size() - 1] == '/';
    if (target_ends_with_slash)
        return this->index_module();
    
    if (this->is_cgi_extension())
        return this->cgi_module();
    
    return this->file_module();
}

int Response::remove_target()
{
    if (remove (this->target.c_str()) == 0)
        return SUCCESS;
    else
        return FAILURE;
}

void Response::delete_module()
{
    std::cerr << "DELETE: " << this->target << std::endl;
    
    if (this->remove_target() == SUCCESS)
    {
        this->response_code = 200;
        this->extension = "html";
        this->response.assign(Response::delete_response.begin(), Response::delete_response.end());
        this->build_headers();
    }
    else
        return this->error_module(500);
}

void Response::build_target()
{
    bool root_uri_is_absolute = this->req.config.root[0] == '/';
    if (root_uri_is_absolute == false)
        this->target = "./";
    
    this->target += this->req.config.root;
    
    bool root_uri_with_trailing_slash = this->req.config.root[this->req.config.root.size() -1] == '/';
    if (root_uri_with_trailing_slash == false)
        this->target += "/";
    
    size_t position_of_location = this->req.req_line.target.find(this->req.matched_loc.getPath());
    size_t size_of_location = this->req.matched_loc.getPath().size();
    std::string target_uri_without_location_part = this->req.req_line.target.substr(position_of_location + size_of_location);
    this->target += target_uri_without_location_part;
    
    std::cout << "target is: " << this->target << std::endl;
}

void Response::build_response()
{
    bool error_parsing_request = this->req.error_code != 0;
    if (error_parsing_request)
        return this->error_module(this->req.error_code);

    this->build_target();
    std::string method = this->req.req_line.method;
    if (method == "GET")
        return this->get_module();
    if (method == "POST")
        return this->cgi_module();
    if (method == "DELETE")
        return this->delete_module();
    return this->error_module(NOT_IMPLEMENTED);
}

bool Response::is_cgi_extension()
{
    if (std::find(this->req.config.cgi_allowed_ext.begin(), this->req.config.cgi_allowed_ext.end(), this->extension) != this->req.config.cgi_allowed_ext.end())
        return true;
    return false;
}

int Response::check_target_is_directory()
{
    int ret = uri_is_directory(this->target);
    if (ret == ERROR)
    {
        this->check_errno_and_send_error(errno);
        return FAILURE;
    }
    if (ret == NO)
    {
        this->error_module(NOT_FOUND);
        return FAILURE;
    }
    return SUCCESS;
}

std::string Response::build_index_uri(std::string index_string)
{
    std::string index_uri;
    // bool index_uri_absolute = index_string[0] == '/';
    // if (index_uri_absolute)
    //     index_uri = "./" + this->req.config.root + index_string;
    // else
    index_uri =  this->target + index_string;
    return index_uri;
}

int Response::try_index_directive()
{
    for (std::vector<std::string>::const_iterator it = this->req.config.index.begin(); it != this->req.config.index.end(); it++)
    {
        std::string index_target = this->build_index_uri(*it);
        if (uri_exists(index_target) == YES)
        {
            this->target = index_target;
            this->get_target_extension();
            this->file_module();
            return SUCCESS;
        }
    }
    return FAILURE;
}

void Response::autoindex_module()
{
    std::cout << "autoindex module" << this->target << std::endl;
    Autoindex ind(this->req);
    std::string auto_index;
    if (ind.genAutoindex(this->target) == SUCCESS)
        auto_index = ind.getAutoindex();
    else
        return error_module(INTERNAL_ERROR);
    this->response.assign(auto_index.begin(), auto_index.end()); 
    this->response_code = OK;
    this->extension = "html";
    this->build_headers();
}

void Response::index_module()
{
    std::cout << "index module" << this->target << std::endl;
    if (this->check_target_is_directory() == FAILURE)
        return;
    
    if (this->try_index_directive() == SUCCESS)
        return;
    
    bool auto_index_is_off = this->req.config.autoindex == 0;
    if (auto_index_is_off)
        return this->error_module(NOT_FOUND);
    else
        return this->autoindex_module();
}

void Response::store_default_error_message()
{
    std::string buf = "Default error page";
    this->extension = "txt";
    this->response.assign(buf.begin(), buf.end());
}

void Response::error_module(int error_code)
{
    std::cout << "error module" << this->target << std::endl;
	
    std::map<int, std::string> error_pages = this->req.getErrorPages();
    bool error_page_exists = !error_pages[error_code].empty();

    if (error_page_exists)
    {
        this->target = error_pages[error_code];
        this->get_target_extension();
        if (this->read_and_store_target_content() == ERROR)
            this->store_default_error_message();
    }
    else
        this->store_default_error_message();
    this->response_code = error_code;
    return this->build_headers();
    
}

void Response::get_target_extension()
{
    size_t pos = this->target.find_last_of('.');
    if (pos != std::string::npos)
        this->extension = this->target.substr(pos + 1);
}

int uri_is_directory(std::string uri)
{
    struct stat buffer;
    if (stat(uri.c_str(), &buffer) == -1)
        return ERROR;
    if (S_ISDIR(buffer.st_mode))
        return YES;
    return NO;
}

int uri_exists(std::string uri)
{
    struct stat buffer;
    if (stat(uri.c_str(), &buffer) == -1)
        return NO;
    return YES;
}

int Response::handle_directory_target_with_no_trailing_slash()
{
    int ret = uri_is_directory(this->target);
    if (ret == ERROR)
    {
        this->check_errno_and_send_error(errno);
        return DONE;
    }
    if (ret == YES)
    {
        this->target += "/";
        index_module(); //could also do a redirection: return 30x and header location
        return DONE;
    }
    return CONTINUE;
}

void Response::check_errno_and_send_error(int error_num)
{
    if (error_num == EACCES)
        return this->error_module(413);
    if (error_num == ENOENT)
        return this->error_module(404);
    return this->error_module(INTERNAL_ERROR);
}

int Response::read_and_store_target_content()
{
    std::ifstream ifs(this->target.c_str(), std::ios::in | std::ios::binary); // OK to open everything in binary mode ?
    if (ifs.fail())
        return ERROR;
    try
    {
        this->response.assign(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
    }
    catch(const std::exception& e) // exception thrown here if target is directory
    {
        return ERROR;
    }
    ifs.close();
    return SUCCESS;
}

void Response::file_module()
{
    
    std::cout << "file module:" << this->target << std::endl;
    
    if (this->handle_directory_target_with_no_trailing_slash() == DONE)
        return;
    if (this->read_and_store_target_content() == ERROR)
    {
        this->check_errno_and_send_error(errno);
        return;
    }
    if (this->response_code == UNSET)
        this->response_code = OK;
    this->build_headers();
}

/*void Response::cgi_module()
{
    // std::cerr << "TARGET: " << this->req.req_line.target << std::endl;
    // std::cerr << "CONFIG: " << this->req.config.cgi_path << std::endl;
    CgiHandler                  cgi(this->req);

    std::cout << "cgi module" << std::endl;
    if (cgi.execScript(this->target) == SUCCESS)
    {
        this->headers = cgi.getHeaders();
        // if no Content-type header, return error 500
        std::string buf = "Content-Type";
        if (std::search(this->headers.begin(), this->headers.end(), buf.begin(), buf.end(), my_equal) == this->headers.end())
            return this->error_module(500);

        this->response_code = 200;
        this->response = cgi.getBody();
        this->build_headers();
    }
    else
        this->error_module(500);
}*/

void Response::cgi_module()
{
    CgiHandler                  cgi(this->req);
    std::vector<unsigned char>  cgi_body;
    std::string                 cgi_headers;

    if (cgi.execScript(this->target) == SUCCESS)
    {
        cgi_headers = cgi.getHeaders();
        cgi_body = cgi.getBody();
        if (!cgi.getStatus().empty())
        {
            this->headers = "HTTP/1.1"; // devrait etre le protocole de la requete ?
            this->headers += cgi.getStatus();
            this->headers += CRLF;
        }
        else if (cgi.getHasRedir() == true)
            this->headers = "HTTP/1.1 302 FOUND\r\n";
        else
            this->headers = "HTTP/1.1 200 OK\r\n";
        if (cgi.getHasContentLength() == false)
        {
            this->headers += "Content-Length: ";
            this->headers += iToString(cgi_body.size());
            this->headers += CRLF;
        }
        if (cgi.getHasContentType() == false && cgi.getHasRedir() == false)
        {
            this->headers += "Content-Type: application/octet-stream";
            this->headers += CRLF;
        }
        this->headers += cgi_headers;
        this->response.assign(this->headers.begin(), this->headers.end());
        this->response.insert(this->response.end(), cgi_body.begin(), cgi_body.end());

        std::cout << "Contenu de la reponse:" << std::endl;
        std::cout << "---------------------------" << std::endl;
        for (size_t i = 0; i < this->response.size(); ++i)
        {
            std::cout << this->response[i];
        }
        std::cout << std::endl;
        std::cout << "---------------------------" << std::endl;
    }
    else
        this->error_module(500);
}
