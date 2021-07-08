#ifndef SERVER_BLOCK_HPP
# define SERVER_BLOCK_HPP

# include "HttpBlock.hpp"
# include "LocationBlock.hpp"

typedef std::map<std::string, LocationBlock>	LocMap;

class ServerBlock : public HttpBlock {

private:
	unsigned int	_listenIP; // s_addr is unsigned int --> /!\ in net byte order
	unsigned short	_listenPort; // sin_port is unsigned short --> /!\ in net byte order
	stringVec		_server_names;
	LocMap			_locations; /*Several LocationBlocks in a ServerBlock */

public:
	ServerBlock(void);
	ServerBlock(ServerBlock const & copy);
	~ServerBlock(void);
	ServerBlock& operator=(ServerBlock const & rhs);

	
	// Setters
	int		setListenIp(std::string ip);
	int		setListenPort(std::string const& port);
	void	setServerNames(strVecIterator first, strVecIterator last);

	std::pair<LocMap::iterator,bool>	addLocation(std::string const& path);

	// Getters
	const unsigned int&		getListenIP(void) const;
	const unsigned short&	getListenPort(void) const;
	const stringVec&		getServerNames(void) const;
	const LocMap&			getLocations(void) const;

};

std::ostream & operator<<(std::ostream & o, ServerBlock const & rhs);

#endif // SERVER_BLOCK_HPP
