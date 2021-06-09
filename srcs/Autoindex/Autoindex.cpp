#include "Autoindex.hpp"

Autoindex::Autoindex(Request const& req) : _req(req)
{
	return ;
}

/*Autoindex::Autoindex(Autoindex const & copy)
{

}*/

Autoindex::~Autoindex(void)
{
	return ;
}

/*Autoindex::Autoindex& operator=(Autoindex const & rhs)
{

}
*/

void		Autoindex::genLink(char *d_name, unsigned char d_type)
{
	this->_buf += "<a href=http://";
	this->_buf += this->_req.host_uri;
	if (!this->_req.host_port.empty())
	{
		this->_buf += ':';
		this->_buf += this->_req.host_port;
	}
	this->_buf += '/';
	this->_buf +=  d_name;
	if (d_type == DT_DIR)
		this->_buf += '/';
	this->_buf += '>';
	this->_buf +=  d_name;
	if (d_type == DT_DIR)
		this->_buf += '/';
	this->_buf += "</a>";
}


int	Autoindex::genAutoindex(std::string const& path) //, std::string const& host, std::string const& port)
{
	DIR					*dir;
	struct dirent		*dir_content;
	struct stat			sb;
	std::string			file_path;
	std::string			title("Index of " + this->_req.req_line.target);
	char				formated_date[18];
	char				prev[4] = "..\0";
	std::stringstream 	ss;

	this->_buf = "<html>\n<head>\t<title>" + title + "</title>\n</head>\n<h1>" + title + "</h1>\n<hr>\n<pre>\n";

	dir = opendir(path.c_str());
	if (dir == NULL)
		return (FAILURE);
	this->genLink(prev, DT_DIR);
	this->_buf += '\n';
	while ((dir_content = readdir(dir)))
	{
		if (strcmp(dir_content->d_name, ".") != 0 && strcmp(dir_content->d_name, "..") != 0)
		{
			this->genLink(dir_content->d_name, dir_content->d_type);
			file_path = path + dir_content->d_name;
			if (stat(file_path.c_str(), &sb) == -1)
				return (FAILURE);
			std::tm *ptm = std::localtime(&sb.st_mtime);
			std::strftime(formated_date, 18, "%d-%b-%Y %H:%M", ptm);
			if (dir_content->d_type != DT_DIR)
				this->_buf += std::string(50 - strlen(dir_content->d_name), ' ');
			else
				this->_buf += std::string(50 - strlen(dir_content->d_name) - 1, ' ');
			this->_buf += formated_date;
			this->_buf += std::string(17, ' ');
			if (dir_content->d_type != DT_DIR)
			{
				ss << sb.st_size;
				this->_buf += ss.str();
				ss.str("");
				ss.clear();
			}
			else
				this->_buf += '-';
			this->_buf += '\n';
		}
	}
	this->_buf += "</pre>\n<hr>\n<p>BB & JN Webserv</p></body>\n</html>\n";

	if (closedir(dir) == -1)
		return (FAILURE);

	return (SUCCESS);
}

const std::string& Autoindex::getAutoindex(void) const
{
	return this->_buf;
}
