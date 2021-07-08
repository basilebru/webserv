#include "Response.hpp"
# include <unistd.h> // stat
#include <sys/types.h> // stat
#include <sys/stat.h> // stat


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
    mp[204] = "No Content";
    mp[301] = "Moved permanently";
    mp[400] = "Bad Request";
    mp[403] = "Forbidden";
    mp[404] = "Not found";
    mp[405] = "Method not allowed";
    mp[413] = "Request entity too large";
    mp[414] = "URI too long";
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

bool my_equal(char c1, char c2)
{
    if (toupper(c1) == toupper(c2))
        return true;
    return false;
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

int uri_is_file(std::string uri)
{
    struct stat buffer;
    if (stat(uri.c_str(), &buffer) == -1)
        return ERROR;
    if (S_ISREG(buffer.st_mode))
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

/* getters */

const std::string& Response::getTarget(void) const
{
    return this->target;
}