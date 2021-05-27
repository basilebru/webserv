#include "Autoindex.hpp"
#include "dirent.h"

Autoindex::Autoindex(void)
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

void	Autoindex::addFileToBuff(std::string const& path)
{
	std::ifstream file;
	file.open(path.c_str());
	if (file.fail())
	{
		return ;
	}
	std::string line;
	while (getline(file, line))
	{
		this->buf += line;
		this->buf += '\n';
	}
	file.close();
}

void	Autoindex::genAutoindex(std::string const& path)
{
	DIR				*dir;
	struct dirent	*dir_content;

	this->addFileToBuff("../../html/header.html");

	dir = opendir(path.c_str());
	while ((dir_content = readdir(dir)))
	{
		this->buf += "<p>";
		this->buf +=  dir_content->d_name;
		this->buf += "</p>\n";
	}
	closedir(dir);
	

	this->buf += "</body>\n</html>\n";

	// struct dirent *dir;

	// dir = readdir(DIR *dir);

	std::cout << this->buf << std::endl;
}
