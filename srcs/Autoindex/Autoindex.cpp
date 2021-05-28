#include "Autoindex.hpp"

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

/*void	Autoindex::addFileToBuff(std::string const& path)
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
}*/

void	Autoindex::genAutoindex(std::string const& path)
{
	DIR				*dir;
	struct dirent	*dir_content;
	struct stat		sb;
	std::string		file_path;
	std::string 	title("Index of " + path);
	char 			formated_date[18];
	std::stringstream ss;


	this->buf = "<html>\n<head>\t<title>" + title + "</title>\n</head>\n<h1>" + title + "</h1>\n<hr>\n<pre>\n";

	dir = opendir(path.c_str());
	while ((dir_content = readdir(dir)))
	{
		if (strcmp(dir_content->d_name, ".") != 0)
		{
			this->buf += "<a href=\"\">";
			this->buf +=  dir_content->d_name;
			this->buf += "</a>";
			if (strcmp(dir_content->d_name, "..") != 0)
			{
				file_path = path + dir_content->d_name;
				if (stat(file_path.c_str(), &sb) == -1)
					return ;
				std::tm *ptm = std::localtime(&sb.st_mtime);
				std::strftime(formated_date, 18, "%d-%b-%Y %H:%M", ptm);
				this->buf += std::string(50 - strlen(dir_content->d_name), ' ');
				this->buf += formated_date;
				this->buf += std::string(17, ' ');
				ss << sb.st_size;
				this->buf += ss.str();
				ss.str("");
				ss.clear();
			}
			this->buf += '\n';
		}
	}
	closedir(dir);
	

	this->buf += "</pre>\n<hr>\n<p>BB & JN Webserv</p></body>\n</html>\n";

	// struct dirent *dir;

	// dir = readdir(DIR *dir);

	std::cout << this->buf << std::endl;
}
