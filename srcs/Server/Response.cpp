#include "Response.hpp"

Response::Response(const Request &req, std::string &buf): req(req), buf(buf)
{
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
        // A. CGI MODULE
        // std::cerr << "TARGET: " << this->req.req_line.target << std::endl;
        // std::cerr << "CONFIG: " << this->req.config.cgi_path << std::endl;

        if (this->req.req_line.target.find(this->req.config.cgi_path) != std::string::npos)
        {
            this->exec_cgi();
            return;
        }
        else if (this->req.req_line.target.find("/favicon.ico") != std::string::npos) //Test pour l'envoi de l'icone du site que le navigateur demande systematiquement
        {
            this->buf = "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n" ;
            return;
        }

        // B. INDEX / AUTOINDEX MODULE
        this->buf = "HTTP/1.1 200 OK\r\n";
        this->buf += "Content-type:text/html\r\n";
        this->buf += "Connection: keep-alive\r\n\r\n";
        if (this->req.target_uri[this->req.target_uri.size() - 1] == '/')
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
                return ;
            }
            this->buf = "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n" ;
            return ;
        }
        
        // C. GET FILE Module
        std::ifstream file;
        file.open(this->req.target_uri.c_str());
        std::cout << this->req.target_uri.c_str() << std::endl;
        if (file.fail())
        {
            this->buf = "Can't open file\n";
            return ;
        }
        std::string line;
        while (getline(file, line))
        {
            this->buf += line;
            this->buf += '\n';
        }
    }

    else
        this->buf = "still working on it :)\n";
    // test big buffer (multiple select calls)
    // this->buf.assign(9000000, 'a');
    // this->buf.push_back('\n');
    // std::cerr << "BUF: " << this->buf << std::endl;
}

void Response::exec_cgi(void)
{
    CgiHandler  cgi(this->req);
    std::string bdy;

    this->buf = "HTTP/1.1 200 OK\r\n";
    bdy = cgi.execScript("./cgi-bin/test.php"); //Il faudra extraire le nom du bin à exécuter depuis la target
    if (!bdy.empty())
        this->buf += bdy;
    else
        this->buf = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
}
