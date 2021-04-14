/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpBlock.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 16:29:31 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/14 17:35:49 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_BLOCK_HPP
# define HTTP_BLOCK_HPP

# include <string>
# include <iostream>
# include <map>
# include <vector>
# include "ServerBlock.hpp"
# include "webserv.hpp"

# define ON		true
# define OFF	false

class HttpBlock {

	typedef size_t								size_type;
	typedef std::vector<std::string>::iterator	strVecIterator;

private:
	std::string						_root;
	bool							_autoindex; // on | off
	std::vector<std::string>		_index;
	std::vector<std::string>		_limit_except; //ou allow_methods ?
	std::map<int, std::string>		_error_pages; /* num error, error file path */
	size_type						_client_max_body_size;
	size_type						_keepalive_timeout;
	bool							_chunked_transfer_encoding; // on | off
	std::vector<std::string>		_auth_basic; // string | off + httpasswd paths
	std::vector<ServerBlock>		_servers;  /*Plusieurs ServerBlocks possibles dans l'HttpBlock */

public:
	HttpBlock(void);
	HttpBlock(HttpBlock const & copy);
	~HttpBlock(void);
	HttpBlock& operator=(HttpBlock const & rhs);

	// Setters
	void	setRoot(std::string path);
	void	setAutoIndex(std::string state);
	void	setIndex(strVecIterator first, strVecIterator last);
	void	setLimitExcept(std::string state);
	void	setErrorPages(strVecIterator first, strVecIterator last, std::string& val);
	void	setMaxBdySize(size_type size);
	void	setKeepaliveTimeout(size_type timeout);
	void	setChunkedEncoding(char state);
	void	setAuthBasic(std::string value);
	// void	setServers(std::string index);

	// Getters
	std::vector<ServerBlock>& getServers();
	ServerBlock&	getLastServer(void);

	
	void	addServer();
	

	ServerBlock& selectServer(std::string ip, int port, std::string host);


};

std::ostream & operator<<(std::ostream & o, HttpBlock const & rhs);

#endif // HTTP_BLOCK_HPP
