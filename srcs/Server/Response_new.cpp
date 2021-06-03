#include "Response.hpp"
#include <iterator>

Response::Response(const Request &req, std::vector<unsigned char> &buf): req(req), response(buf)
{}

Response::~Response(void)
{
    std::cerr << "RESPONSE DESTRUCTOR" << std::endl;
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
        this->buf = "HTTP/1.1 200 OK\r\n";
        this->buf += "Content-type:text/html\r\n";
        this->buf += "Content-Length: 10\r\n"; // A calculer
        this->buf += "Connection: keep-alive\r\n\r\n";


        // A. CGI MODULE
        // std::cerr << "TARGET: " << this->req.req_line.target << std::endl;
        // std::cerr << "CONFIG: " << this->req.config.cgi_path << std::endl;

        // if (this->req.req_line.target.find(this->req.config.cgi_path) != std::string::npos)
        // {
        //     this->exec_cgi("./cgi-bin/image.pl");
        //     return;
        // }
        // else if (this->req.req_line.target.find("/favicon.ico") != std::string::npos) //Test pour l'envoi de l'icone du site que le navigateur demande systematiquement
        // {
        //     // this->buf = "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n" ;
        //     this->send_img("./html/images/favicon.ico");
        //     return;
        // }
        // else if (this->req.req_line.target.find("/image") != std::string::npos) //Test pour l'envoi de l'icone du site que le navigateur demande systematiquement
        // {
        //     this->send_img("/home/julien/Cursus42/webserv/html/images/favicon.ico");
        //     return;
        // }

        // if cgi extension
        // -> go cgi
        if (this->req.target_uri[this->req.target_uri.size() - 1] == '/')
            this->index_module();
        else
            this->get_file_module();
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

void Response::get_file_module()
{
    std::ifstream ifs(path.c_str(), std::ios::in | std::ios::binary);

    this->buf = "HTTP/1.1 200 OK\r\n";
    this->buf += "Accept-Ranges: bytes\r\n";
    this->buf += "Content-Length: 1150\r\n";
    this->buf += "Content-Type: image/vnd.microsoft.icon\r\n";
    this->buf += "Connection: keep-alive\r\n\r\n";

    this->response.assign(this->buf.begin(), this->buf.end());
    this->response.insert(this->response.end(), std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());

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

void Response::send_img(std::string const& path)
{
}

void Response::exec_cgi(std::string const& path)
{
    CgiHandler                  cgi(this->req);
    std::vector<unsigned char>  cgi_output;

    cgi_output = cgi.execScript(path); //Il faudra extraire le nom du bin à exécuter depuis la target
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
        this->buf = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
        this->response.assign(this->buf.begin(), this->buf.end());
    }
}
