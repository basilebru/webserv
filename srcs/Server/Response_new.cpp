#include "Response_new.hpp"
#include <iterator>

Response::Response(const Request &req, std::vector<unsigned char> &buf): req(req), response(buf)
{
    this->response_code = this->req.error_code;
    this->extension = this->req.req_line.extension;
    this->target = this->req.target_uri;
    // std::cout << "Response created" << std::endl;
    // this->body = new std::vector<unsigned char>;
    // this->headers = new std::vector<unsigned char>;
}

Response::~Response(void)
{
    // delete body;
    // delete headers;
}

int Response::process()
{
    if (this->req.connection_end() || this->req.get_error_code())
    {
        // log message (debug)
        if (this->req.connection_end())
            std::cout << RED << "Client closed connection" << NOCOLOR << std::endl;
        else
            std::cout << RED << "Request error, closing connection" << NOCOLOR << std::endl;
        return -1;
    }
    if (this->req.request_is_ready())
    {
        std::cout << "Request ready to be treated" << std::endl;
        this->build_response();
        return 1;
    }
    return 0;
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
    // headers
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

    if (this->req.req_line.method == "GET")
    {
        if (this->target[this->target.size() - 1] == '/')
            this->index_module();
        else
            this->file_module();
    }
    else
    {
        this->error_module(500);
    }
    this->build_headers();
}

bool Response::is_cgi_extension()
{
    if (std::find(this->req.config.cgi_allowed_ext.begin(), this->req.config.cgi_allowed_ext.end(), this->extension) != this->req.config.cgi_allowed_ext.end())
        return true;
    return false;
}

void Response::index_module()
{
    // check if directory exists // better to make a check with stat ? https://stackoverflow.com/questions/18100097/portable-way-to-check-if-directory-exists-windows-linux-c
    DIR *dir;
    if (!(dir = opendir(this->target.c_str())))
    {
        this->error_module(404);
        return ;
    }
    closedir(dir);

    struct stat buffer;
    for (std::vector<std::string>::const_iterator it = this->req.config.index.begin(); it != this->req.config.index.end(); it++)
    {
        // std::cout << "index: " << *it << std::endl;
        std::string target;

        target =  "./" + this->req.config.root + '/' + *it;
        // std::cout << "target: " << this->target << std::endl; 
        if (stat(target.c_str(), &buffer) == 0)
        {
            size_t pos;
            pos = it->find('.');
            if (pos != std::string::npos)
                this->extension = it->substr(pos + 1);
            // std::cout << "ext:" << this->extension << std::endl;
            this->target = target;
            return this->file_module();
        }
    }
    if (this->req.config.autoindex == 1)
    {
        std::cout << "auto" << std::endl;
        Autoindex ind;
        std::string auto_index = ind.genAutoindex(this->target);
        this->response.assign(auto_index.begin(), auto_index.end()); // TO DO: gestion des "erreurs": cas ou genAutoindex renvoie "" (dossier qui n'existe pas...)
        this->response_code = 200;
        this->extension = "html";
        return ;
    }
    this->error_module(404);
}

void Response::error_module(int error_code)
{
    // look into conf error_codes to fill body

    this->response_code = error_code;
    std::string buf;
    buf = "Error ";
    buf += iToString(error_code);
    this->response.assign(buf.begin(), buf.end());
    this->extension = "html";
}

void Response::file_module()
{
    std::ifstream ifs(this->target.c_str(), std::ios::in | std::ios::binary); // OK to open everything in binary mode ?
    if (ifs.fail())
    {
        this->error_module(404);
        return ;
    }
    if (this->is_cgi_extension())
        this->cgi_module();
    else
    {
        this->response_code = 200;
        this->response.assign(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
    }
    ifs.close();
}

void Response::cgi_module()
{
    // std::cerr << "TARGET: " << this->req.req_line.target << std::endl;
    // std::cerr << "CONFIG: " << this->req.config.cgi_path << std::endl;
    CgiHandler                  cgi(this->req);

    std::cout << "cgi module" << std::endl;
    if (cgi.execScript(this->target) == SUCCESS)
    {
        this->response_code = 200;
        this->headers = cgi.getHeaders();

        // if no Content-type header, return error 500
        std::string buf = "Content-Type";
        if (std::search(this->headers.begin(), this->headers.end(), buf.begin(), buf.end(), my_equal) == this->headers.end())
        {
            std::cout << "ct" << std::endl;
            this->error_module(500);
            return ;
        }
        this->response = cgi.getBody();
    }
    else
    {
        this->error_module(500);
    }
}
