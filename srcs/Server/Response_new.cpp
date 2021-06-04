#include "Response_new.hpp"
#include <iterator>

Response::Response(const Request &req, std::vector<unsigned char> &buf): req(req), response(buf)
{
    this->response_code = this->req.error_code;
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
void Response::build_headers()
{
    std::string buf;
    
    // response line
    buf = "HTTP/1.1 ";
    buf += iToString(this->response_code);
    buf += " OK?\r\n"; // look into a map to get signigication of response_code

    // headers
    buf += "Content-length: ";
    buf += iToString(this->response.size());
    buf += "\r\n";
    if (this->req.target_uri.find("images") != std::string::npos)
        buf += "Content-Type: image/vnd.microsoft.icon\r\n";
    // buf += "Accept-Ranges: bytes\r\n";
    buf+= "\r\n";
    this->response.insert(this->response.begin(), buf.begin(), buf.end());
}

void Response::build_response()
{

    if (this->req.req_line.method == "GET")
    {
        if (this->req.target_uri[this->req.target_uri.size() - 1] == '/')
            this->index_module();
        else
            this->file_module();
        this->build_headers();
    }

    else
    {
        this->error_module(500);
    }
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
    // check if directory exists
    DIR *dir;
    if (!(dir = opendir(this->req.target_uri.c_str())))
    {
        this->error_module(404);
        return ;
    }
    closedir(dir);

    // if (!this->req.config.index.empty())
    // {
    //     // try to GET index
    //     return ;
    // }
    if (this->req.config.autoindex == 1)
    {
        Autoindex ind;
        std::string auto_index = ind.genAutoindex(this->req.target_uri);
        this->response.assign(auto_index.begin(), auto_index.end()); // TO DO: gestion des "erreurs": cas ou genAutoindex renvoie "" (dossier qui n'existe pas...)
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
        this->response_code = 200;
        this->response.assign(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
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
        std::cerr << "cgi_output-Size: " << cgi_output.size() << std::endl;
        this->response = cgi_output;
        // std::cerr << "cgi_output: " << cgi_output << std::endl;
    }
    else
    {
        this->error_module(500);
        // this->buf = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
        // this->response.assign(this->buf.begin(), this->buf.end());
    }
}
