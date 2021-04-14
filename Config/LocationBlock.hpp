/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationBlock.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 15:59:09 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/14 17:21:05 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_BLOCK_HPP
# define LOCATION_BLOCK_HPP

# include <string>
# include <iostream>
# include <vector>
# include <map>
# include "webserv.hpp"


class LocationBlock {

	typedef size_t									size_type;
	typedef std::map<std::string, std::string>		mapCgiParams;
	typedef std::vector<std::string>::iterator		strVecIterator;

private:
	std::string						_path;
	std::string						_root;
	bool							_autoindex; // on | off
	std::vector<std::string>		_index;
	std::vector<std::string>		_limit_except; //ou allow_methods ?
	std::map<int, std::string>		_error_pages; /* num error, error file path */
	size_type						_client_max_body_size;
	size_type						_keepalive_timeout;
	bool							_chunked_transfer_encoding; // on | off
	std::vector<std::string>		_auth_basic; // string | off + httpasswd paths

	/*For CGI*/
	std::string						_cgi_pass;
	size_type						_cgi_port; //mixer en std::string avec _cgi_pass ?
	std::string						_cgi_index;
	mapCgiParams					_cgi_params;

public:
	LocationBlock(void);
	LocationBlock(LocationBlock const & copy);
	~LocationBlock(void);
	LocationBlock& operator=(LocationBlock const & rhs);

	// Setters
	void	setPath(std::string path);
	void	setRoot(std::string path);
	void	setAutoIndex(std::string state);
	void	setIndex(strVecIterator first, strVecIterator last);
	void	setLimitExcept(std::string method);
	void	setErrorPages(strVecIterator first, strVecIterator last, std::string& val);
	void	setMaxBdySize(size_type size);
	void	setKeepaliveTimeout(size_type timeout);
	void	setChunkedEncoding(char state);
	void	setAuthBasic(std::string value);
	
	void	setCgiPass(std::string value);
	void	setCgiPort(size_type port);
	void	setCgiIndex(std::string index);
	void	setCgiParams(mapCgiParams params);

};

std::ostream & operator<<(std::ostream & o, LocationBlock const & rhs);

#endif // LOCATION_BLOCK_HPP
