#ifndef AUTOINDEX_HPP
# define AUTOINDEX_HPP

# include "webserv.hpp"

class Autoindex {

private:
	std::string	buf;

	void	addFileToBuff(std::string const& path);


public:
	Autoindex(void);
	Autoindex(Autoindex const & copy);
	~Autoindex(void);
	Autoindex& operator=(Autoindex const & rhs);

	void	genAutoindex(std::string const& path);

};

std::ostream & operator<<(std::ostream & o, Autoindex const & rhs);

#endif // AUTOINDEX_HPP
