#include "Response_new.hpp"
#include <iterator>

Response::Response(const Request &req, std::vector<unsigned char> &buf): req(req), response(buf)
{
    std::cout << "Response created" << std::endl;
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

void Response::build_response()
{

    if (this->req.req_line.method == "GET")
    {
        if (this->req.target_uri[this->req.target_uri.size() - 1] == '/')
            this->index_module();
        else
            this->file_module();
        // this->build_headers();
    }

    else
    {
        this->buf = "still working on it :)\n";
        this->response.assign(this->buf.begin(), this->buf.end());
    }
    // test big buffer (multiple select calls)
    // this->buf.assign(9000000, 'a');
    // this->buf.push_back('\n');
    std::cerr << "BUF: " << this->buf << std::endl;
}

bool Response::is_cgi_extension()
{
    std::string ext = this->req.target_uri.substr(this->req.target_uri.find_last_of('.'));
    
    std::cout << "ext " << ext << std::endl;
    if (ext == ".php") // chercher dans le param cgi_ext de config
        return true;
    return false;
}

void Response::index_module()
{
    // if (!this->req.config.index.empty())
    // {
    //     // try to GET index
    //     return ;
    // }
    if (this->req.config.autoindex == 1)
    {
        std::cout << "auto" << std::endl;
        Autoindex ind;
        this->buf += ind.genAutoindex(this->req.target_uri); // TO DO: gestion des "erreurs": cas ou genAutoindex renvoie "" (dossier qui n'existe pas...)
        this->response.assign(this->buf.begin(), this->buf.end());
        // displayVecAsString(this->response);
        // std::cerr << "BUF: " << this->buf << std::endl;
        return ;
    }
    this->buf = "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n" ;
    this->response.assign(this->buf.begin(), this->buf.end());
}

void Response::error_module(int error_code)
{
    std::stringstream ss;
    ss << error_code;
    this->buf = "HTTP/1.1 ";
    this->buf += ss.str();
    this->buf += " Error \r\n";
    this->buf += "Content-Length: 9\r\n\r\n";
    this->buf += "Error ";
    this->buf += ss.str();
    this->response.assign(this->buf.begin(), this->buf.end());

}

void Response::file_module()
{
       
    std::ifstream ifs(this->req.target_uri.c_str(), std::ios::in | std::ios::binary); // OK to open everything in binary mode ?
    if (ifs.fail())
    {
        this->error_module(404);
        return ;
    }
    if (this->is_cgi_extension())
        this->cgi_module();
    else
    {
        this->buf = "HTTP/1.1 200 OK\r\n";
        this->buf += "Accept-Ranges: bytes\r\n";
        this->buf += "Content-Length: 1150\r\n";
        this->buf += "Content-Type: image/vnd.microsoft.icon\r\n";
        this->buf += "Connection: keep-alive\r\n\r\n";
        this->response.assign(this->buf.begin(), this->buf.end());
        this->response.insert(this->response.end(), std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
    }

    ifs.close();

    // std::ifstream file;
    // file.open(this->req.target_uri.c_str());
    // std::cout << this->req.target_uri.c_str() << std::endl;
    // if (file.fail())
    // {
    //     this->buf = "Can't open file\n";
    //     this->response.assign(this->buf.begin(), this->buf.end());
    //     return ;
    // }
    // std::string line;
    // while (getline(file, line))
    // {
    //     this->buf += line;
    //     this->buf += '\n';
    // }
    // this->response.assign(this->buf.begin(), this->buf.end());
}

// void Response::send_img(std::string const& path)
// {
// }

void Response::cgi_module()
{
    // std::cerr << "TARGET: " << this->req.req_line.target << std::endl;
    // std::cerr << "CONFIG: " << this->req.config.cgi_path << std::endl;
    CgiHandler                  cgi(this->req);
    std::vector<unsigned char>  cgi_output;

    cgi_output = cgi.execScript(this->req.target_uri); //Il faudra extraire le nom du bin à exécuter depuis la target
    if (!cgi_output.empty())
    {
        this->buf = "HTTP/1.1 200 OK\r\n";
        std::cerr << "cgi_output-Size: " << cgi_output.size() << std::endl;
        this->response.assign(this->buf.begin(), this->buf.end());
        this->response.insert(this->response.end(), cgi_output.begin(), cgi_output.end());
        // std::cerr << "cgi_output: " << cgi_output << std::endl;
    }
    else
    {
        this->error_module(500);
        // this->buf = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
        // this->response.assign(this->buf.begin(), this->buf.end());
    }
}
