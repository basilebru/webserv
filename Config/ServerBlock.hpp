/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 15:23:44 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/12 18:25:45 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef SERVER_BLOCK_HPP
# define SERVER_BLOCK_HPP

# include <string>
# include <iostream>
# include <map>
# include <vector>
# include "webserv.hpp"
# include "LocationBlock.hpp"

typedef size_t						size_type;
typedef std::vector<std::string>	stringVec;
typedef std::map<int, std::string>	errorMap;

class ServerBlock {

private:
	std::string						_listenIP;
	size_type						_listenPort;
	stringVec						_server_names;
	bool							_autoindex;
	std::string						_root;
	std::vector<LocationBlock>		_locations; /*Plusieurs LocationBlocks possibles dans un ServerBlock */
	std::map<int, std::string>		_error_pages; /* num error, error file path */
	size_type						_client_max_body_size;
	size_type						_keepalive_timeout;
	stringVec						_index;
	bool							_chunked_transfer_encoding; // on | off
	stringVec						_auth_basic; // string | off + httpasswd paths

public:
	ServerBlock(void);
	ServerBlock(ServerBlock const & copy);
	~ServerBlock(void);
	ServerBlock& operator=(ServerBlock const & rhs);

	
	// Setters
	void	setMaxBdySize(size_type size);
	void	setKeepaliveTimeout(size_type timeout);

	// Getters
	const std::string&					getListenIP(void) const;
	const size_type&					getListenPort(void) const;
	const stringVec&					getServerNames(void) const;
	const bool&							getAutoindex(void) const;
	const std::string&					getRoot(void) const;
	const std::vector<LocationBlock>&	getLocations(void) const;
	LocationBlock&						getLastLocation(void);
	const errorMap&						getErrorPages(void) const;
	const size_type&					getMaxBdySize(void) const;
	const size_type&					getKeepaliveTime(void) const;
	const stringVec&					getIndexes(void) const;
	const bool&							getChunkedEncoding(void) const;
	const stringVec&					getAuthBasic(void) const;

};

std::ostream & operator<<(std::ostream & o, ServerBlock const & rhs);

#endif // SERVER_BLOCK_HPP
