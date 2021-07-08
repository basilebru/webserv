#ifndef AUTOINDEX_HPP
# define AUTOINDEX_HPP

# include "webserv.hpp"
# include "request_class.hpp"
# include "dirent.h"
# include <ctime>

class Autoindex {

private:
	std::string		_buf;
	const Request&	_req;

	void genLink(char *d_name, unsigned char d_type);
	Autoindex(void);


public:
	Autoindex(Request const& req);
	Autoindex(Autoindex const & copy);
	~Autoindex(void);
	Autoindex& operator=(Autoindex const & rhs);

	int					genAutoindex(std::string const& path);
	const std::string&	getAutoindex(void) const;

};

std::ostream & operator<<(std::ostream & o, Autoindex const & rhs);

#endif // AUTOINDEX_HPP
