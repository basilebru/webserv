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

        if (this->req.req_line.target.find("/cgi-bin") != std::string::npos) //Just pour test, il faudrait extraire le nom du bin à exécuter :-)
            this->exec_cgi();
        else
        {
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
    }
    else
        this->buf = "still working on it :)\n";
    // test big buffer (multiple select calls)
    // this->buf.assign(9000000, 'a');
    // this->buf.push_back('\n');
}

void Response::exec_cgi()
{
    CgiHandler cgi;
    std::string ret;

    this->buf = cgi.execScript("./cgi-bin/test.php");
    
}
