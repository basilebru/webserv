# include "Response_new.hpp"
# include "Autoindex.hpp"
# include "CgiHandler.hpp"
# include <iterator>

Response::Response(const Request &req, std::vector<unsigned char> &buf): req(req), response(buf)
{
    this->target = this->req.target_uri;
    this->extension = this->req.req_line.extension;
    this->response_code = 0;
    // std::cout << "Response created" << std::endl;
}

Response::~Response(void)
{
}

int Response::process()
{
    if (this->req.connection_end())
    {
        std::cout << RED << "Client closed connection" << NOCOLOR << std::endl;
        return CLOSE;
    }

    if (this->req.request_is_ready() || this->req.error_code)
    {
        std::cout << "Request ready to be treated" << std::endl;
        try
        {
            this->build_response();
        }
        catch(const std::exception& e)
        {
            this->error_module(500);
            return CLOSE;
        }
        return SEND;
    }

    return SKIP;
}

void Response::build_response_line()
{
    std::string ret;
    ret = "HTTP/1.1 ";
    ret += iToString(this->response_code);
    ret += " STATUS\r\n"; // look into a map to get signigication of response_code
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
    return mp;
}

void Response::build_headers()
{
    bool empty_headers = this->headers.empty();
    
    std::string buf = "Content-Length";
    if (std::search(this->headers.begin(), this->headers.end(), buf.begin(), buf.end(), my_equal) == this->headers.end())
    {
        std::cout << "building content-length" << std::endl;
        std::cout << this->response.size() << std::endl;
        this->headers += "Content-length: ";
        this->headers += iToString(this->response.size());
        this->headers += "\r\n";
    }

    buf = "Content-type";
    if (std::search(this->headers.begin(), this->headers.end(), buf.begin(), buf.end(), my_equal) == this->headers.end())
    {
        // std::cout << "building content-type" << std::endl;
        this->headers += "Content-type: ";
        str_map::iterator it;
        if ((it = this->extension_map.find(this->extension)) != this->extension_map.end())
            this->headers += it->second;
        else
            this->headers += "application/octet-stream"; // ou text/plain ?
        this->headers += "\r\n";
    }

    if (empty_headers)
        this->headers += "\r\n";
    this->response.insert(this->response.begin(), this->headers.begin(), this->headers.end());
    this->build_response_line();
}

void Response::build_response()
{
    if (this->req.error_code)
        return this->error_module(this->req.error_code);
    
    if (this->req.req_line.method == "GET")
    {
        if (this->target[this->target.size() - 1] == '/')
            return this->index_module();
        else
            return this->file_module();
    }
    else
        return this->error_module(500);
}

bool Response::is_cgi_extension()
{
    if (std::find(this->req.config.cgi_allowed_ext.begin(), this->req.config.cgi_allowed_ext.end(), this->extension) != this->req.config.cgi_allowed_ext.end())
        return true;
    return false;
}

void Response::index_module()
{
    // check if dir exists
    struct stat buffer;
    if (stat(this->target.c_str(), &buffer) == -1)
        return this->error_module(500);
    
    // try each file in index directive
    for (std::vector<std::string>::const_iterator it = this->req.config.index.begin(); it != this->req.config.index.end(); it++)
    {
        std::string target =  "./" + this->req.target_uri + *it;
        if (stat(target.c_str(), &buffer) == 0)
        {
            this->target = target;
            this->get_target_extension();
            return this->file_module();
        }
    }
    if (this->req.config.autoindex == 0)
        return this->error_module(404);

    // build autoindex
    std::cout << "auto" << std::endl;
    Autoindex ind;
    std::string auto_index = ind.genAutoindex(this->target);
    if (auto_index.empty())
        return error_module(500);
    this->response.assign(auto_index.begin(), auto_index.end()); 
    this->response_code = 200;
    this->extension = "html";
    this->build_headers();
}

void Response::error_module(int error_code)
{
	std::map<int, std::string> error_pages = this->req.getErrorPages();
	std::string buf;

    this->target = error_pages[error_code];
    this->get_target_extension();
    this->response_code = error_code;

    std::ifstream ifs(this->target.c_str(), std::ios::in);
    if (ifs.fail())
    {
    	this->response_code = 500;
        buf = "500 Internal Server Error";
	    this->extension = "txt";
        this->response.assign(buf.begin(), buf.end());
        return this->build_headers();
    }
    this->file_module();
    // else
    // {
	//     this->extension = "html";
	//     this->response.insert(this->response.end(), std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());

    // }
    // this->build_headers();
	// std::cout << "Contenu de la reponse:" << std::endl;
    // std::cout << "---------------------------" << std::endl;
    // for (size_t i = 0; i < this->response.size(); ++i)
    // {
    //     std::cout << this->response[i];
    // }
    // std::cout << std::endl;
    // std::cout << "---------------------------" << std::endl;
    // ifs.close();
}

void Response::get_target_extension()
{
    size_t pos = this->target.find_last_of('.');
    if (pos != std::string::npos)
        this->extension = this->target.substr(pos + 1);
    std::cout << "ext:" << this->extension << std::endl;
}

void Response::file_module()
{
    if (this->is_cgi_extension())
        return this->cgi_module();
    
    std::ifstream ifs(this->target.c_str(), std::ios::in | std::ios::binary); // OK to open everything in binary mode ?
    if (ifs.fail())
        return this->error_module(404); // depend de errno ?
    else
    {
        if (this->response_code == 0)
            this->response_code = 200;
        this->response.assign(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
    }
    ifs.close();
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
