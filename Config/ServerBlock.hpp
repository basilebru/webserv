/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 15:23:44 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/16 19:52:52 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef SERVER_BLOCK_HPP
# define SERVER_BLOCK_HPP

# include <string>
# include <iostream>
# include <map>
# include <vector>
# include "webserv.hpp"
# include "utils.hpp"
# include "LocationBlock.hpp"

typedef size_t									size_type;
typedef std::vector<std::string>				stringVec;
typedef std::map<int, std::string>				errorMap;
typedef std::map<std::string, LocationBlock>	LocMap;
typedef std::vector<std::string>::iterator		strVecIterator;

class ServerBlock {

private:
	unsigned int					_listenIP; // s_addr is unsigned int
	size_type						_listenPort;
	stringVec						_server_names;
	std::string						_root;
	int								_autoindex;
	stringVec						_indexes;
	stringVec						_limit_except; // GET POST ...
	std::map<int, std::string>		_error_pages; /* num error, error file path */
	long int						_client_max_body_size;
	long int						_keepalive_timeout;
	int								_chunked_transfer_encoding; // on | off
	std::string						_auth_basic; // string | off
	std::string						_auth_basic_user_file; //httpasswd paths
	LocMap							_locations; /*Plusieurs LocationBlocks possibles dans un ServerBlock */

public:
	ServerBlock(void);
	ServerBlock(ServerBlock const & copy);
	~ServerBlock(void);
	ServerBlock& operator=(ServerBlock const & rhs);

	
	// Setters
	int		setListenIp(std::string ip);
	int		setListenPort(size_type port);
	void	setServerNames(strVecIterator first, strVecIterator last);
	void	setRoot(std::string path);
	void	setAutoIndex(std::string& state);
	void	setIndexes(strVecIterator first, strVecIterator last);
	void	setLimitExcept(strVecIterator first, strVecIterator last);
	void	setErrorPages(strVecIterator first, strVecIterator last, std::string& val);
	void	setMaxBdySize(size_type size);
	void	setKeepaliveTimeout(size_type timeout);
	void	setChunkedEncoding(std::string& state);
	void	setAuthBasic(std::string value);
	void	setAuthBasicFile(std::string path);

	std::pair<LocMap::iterator,bool>	addLocation(std::string path);

	// Getters
	const unsigned int&			getListenIP(void) const;
	const size_type&			getListenPort(void) const;
	const stringVec&			getServerNames(void) const;
	const std::string&			getRoot(void) const;
	const int	&				getAutoindex(void) const;
	const stringVec&			getIndexes(void) const;
	const stringVec&			getLimitExcept(void) const;
	const errorMap&				getErrorPages(void) const;
	const long int&				getMaxBdySize(void) const;
	const long int&				getKeepaliveTime(void) const;
	const int	&				getChunkedEncoding(void) const;
	const std::string&			getAuthBasic(void) const;
	const std::string&			getAuthBasicFile(void) const;
	
	const LocMap&				getLocations(void) const;
	// LocationBlock&				getLastLocation(void);

};

std::ostream & operator<<(std::ostream & o, ServerBlock const & rhs);

#endif // SERVER_BLOCK_HPP
