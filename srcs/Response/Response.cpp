# include "Response.hpp"
# include "Autoindex.hpp"
# include "CgiHandler.hpp"
# include <iterator>

Response::Response(const Request &req, std::vector<unsigned char> &buf): req(req), response(buf)
{
    this->extension = this->req.req_line.extension;
    this->response_code = 0;
    this->redir_activated = false;
    // std::cout << "Response created" << std::endl;
}

Response::~Response(void)
{
    // std::cerr << "RESPONSE DESTRUCTOR" << std::endl;
}

// returns CLOSE if connection needs to be closed (error during request or EOF received)
// returns NEW if request has been processed, buffer is filled -> request can be deleted
int Response::build()
{
    // debug infos
    if (req.request_is_ready())
        std::cout << "<<<<<<Request ready to be treated>>>>>" << std::endl;
    if (req.error_code)
        std::cout << "<<<<<<Error found in request>>>>>" << std::endl;
    this->req.print();
    this->req.print_config();
    //

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
    
    if (this->req.error_code)
        return CLOSE;
    else 
        return NEW;
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
    if (method == "PUT")
        return this->put_module();
    return this->error_module(NOT_IMPLEMENTED);
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


void Response::get_module()
{
    const std::pair<int, std::string> return_dir = this->req.config.return_dir;
    bool loc_has_return_directive = !return_dir.second.empty();
    if (loc_has_return_directive == true)
        return this->redir_module(return_dir.first, return_dir.second);

    bool target_ends_with_slash = this->target[this->target.size() - 1] == '/';
    if (target_ends_with_slash)
        return this->index_module();
    
    if (this->is_cgi_extension())
        return this->cgi_module();
    
    return this->file_module();
}

void Response::redir_module(int redir_code, std::string redir_target)
{
    this->redir_activated = true;
    this->response_code = redir_code;
    this->redir_target = redir_target;
    this->build_headers();
}

void Response::error_module(int error_code)
{
    std::cout << "error module" << this->target << std::endl;

    std::map<int, std::string> error_pages = this->req.getErrorPages();
    bool error_page_exists = !error_pages[error_code].empty();

    if (error_page_exists)
    {
        this->target = error_pages[error_code];
        this->set_extension_from_target();
        if (this->read_and_store_target_content() == ERROR)
            this->store_default_error_message();
    }
    else
        this->store_default_error_message();
    this->response_code = error_code;
    return this->build_headers();
}

void Response::store_default_error_message()
{
    std::string buf = "Default error page";
    this->extension = "txt";
    this->response.assign(buf.begin(), buf.end());
}

void Response::put_module()
{

    std::ofstream file;

    if (uri_is_file(this->target) == YES)
    {
        file.open(this->target.c_str(), std::ofstream::binary | std::ofstream::out | std::ofstream::trunc);
        if (file.is_open() == false)
            return this->error_module(INTERNAL_ERROR);
    }
    else
    {
        file.open(this->target.c_str(), std::ofstream::binary | std::ofstream::out);
        if (file.is_open() == false)
            return this->error_module(FORBIDDEN);

    }
    
    file.write((char*)&this->req.body[0], this->req.body.size());
    file.close();
    this->headers += "HTTP/1.1 204 No Content\r\n";
    this->headers += "Content-Location: /file_should_exist_after\r\n"; // a modifier
    // this->build_keep_alive();
    this->headers += CRLF;
    this->response.assign(this->headers.begin(), this->headers.end());

    std::cout << "Contenu de la reponse:" << std::endl;
    std::cout << "---------------------------" << std::endl;
    for (size_t i = 0; i < this->response.size(); ++i)
    {
        std::cout << this->response[i];
    }
    std::cout << "---------------------------" << std::endl;
    return ;
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

int Response::remove_target()
{
    if (remove (this->target.c_str()) == 0)
        return SUCCESS;
    else
        return FAILURE;
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
            this->set_extension_from_target();
            this->file_module();
            return SUCCESS;
        }
    }
    return FAILURE;
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
        std::cout << "directory target with no trailing slash" << std::endl;
        this->redir_module(REDIR, this->req.req_line.target + "/");
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

bool Response::is_cgi_extension()
{
    if (this->req.config.cgi_extensions.count(this->extension))
        return true;
    return false;
}

void Response::cgi_module()
{
    std::vector<unsigned char>  cgi_body;
    std::string                 cgi_headers;

    stringMap cgi_extensions = this->req.getCgi_extensions();
    std::string cgi_path = cgi_extensions[extension];

    if (!cgi_path.empty())
    {
        CgiHandler cgi(this->req, *this);
        if (cgi.execScript(cgi_path) == SUCCESS)
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

            // if (this->response.size() < 1000000)
            // {
            //     std::cout << "Contenu de la reponse:" << std::endl;
            //     std::cout << "---------------------------" << std::endl;
            //     for (size_t i = 0; i < this->response.size(); ++i)
            //     {
            //         std::cout << this->response[i];
            //     }
            //     std::cout << "---------------------------" << std::endl;
            // }
        }
        else
            this->error_module(500);
    }
    else
        this->error_module(204); // dans le cas où l'extension demandée n'est pas gérée et/ou que la map des ext est vide
}

void Response::set_extension_from_target()
{
    size_t pos = this->target.find_last_of('.');
    if (pos != std::string::npos)
        this->extension = this->target.substr(pos + 1);
}


