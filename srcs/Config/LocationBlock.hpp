#ifndef LOCATION_BLOCK_HPP
# define LOCATION_BLOCK_HPP

# include "HttpBlock.hpp"

class LocationBlock : public HttpBlock {

private:
	std::string		_path; //Optional since locations are stored in map
	std::string		_cgi_path; // No need anymore


public:
	LocationBlock(void);
	LocationBlock(LocationBlock const & copy);
	~LocationBlock(void);
	LocationBlock& operator=(LocationBlock const & rhs);

	// Setters
	void	setPath(std::string const& path);
	void	setCgiPath(std::string const& path);

	// Getters
	const std::string&		getPath(void) const;
	const std::string&		getCgiPath(void) const;
};

std::ostream & operator<<(std::ostream & o, LocationBlock const & rhs);

#endif // LOCATION_BLOCK_HPP
