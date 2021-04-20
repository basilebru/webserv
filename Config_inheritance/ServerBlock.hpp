/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 15:23:44 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/20 17:28:09 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef SERVER_BLOCK_HPP
# define SERVER_BLOCK_HPP

# include "webserv.hpp"
# include "utils.hpp"
# include "HttpBlock.hpp"
# include "LocationBlock.hpp"
class LocationBlock;

typedef std::map<std::string, LocationBlock>	LocMap;

class ServerBlock : public HttpBlock {

private:
	unsigned int					_listenIP; // s_addr is unsigned int
	size_type						_listenPort;
	stringVec						_server_names;
	LocMap							_locations; /*Plusieurs LocationBlocks possibles dans un ServerBlock */

public:
	ServerBlock(void);
	ServerBlock(ServerBlock const & copy);
	~ServerBlock(void);
	ServerBlock& operator=(ServerBlock const & rhs);

	
	// Setters
	int		setListenIp(std::string ip);
	int		setListenPort(std::string port);
	void	setServerNames(strVecIterator first, strVecIterator last);

	std::pair<LocMap::iterator,bool>	addLocation(std::string path);

	// Getters
	const unsigned int&			getListenIP(void) const;
	const size_type&			getListenPort(void) const;
	const stringVec&			getServerNames(void) const;
	
	const LocMap&				getLocations(void) const;

};

std::ostream & operator<<(std::ostream & o, ServerBlock const & rhs);

#endif // SERVER_BLOCK_HPP
