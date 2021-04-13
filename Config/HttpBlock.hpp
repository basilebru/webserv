/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpBlock.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 16:29:31 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/13 18:49:23 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_BLOCK_HPP
# define HTTP_BLOCK_HPP

# include <string>
# include <iostream>
# include <map>
# include <vector>
# include "ServerBlock.hpp"

# define ON		true
# define OFF	false

class HttpBlock {

	typedef size_t					size_type;

private:
	bool							_autoindex; // on | off
	bool							_chunked_transfer_encoding; // on | off
	std::string						_root;
	std::map<int, std::string>		_error_pages; /* num error, error file path */
	size_type						_client_max_body_size;
	size_type						_keepalive_timeout;
	std::vector<std::string>		_index;
	// std::vector<std::string>		_includes;
	std::vector<ServerBlock>		_servers;  /*Plusieurs ServerBlocks possibles dans l'HttpBlock */
	std::vector<std::string>		_auth_basic; // string | off + httpasswd paths

public:
	HttpBlock(void);
	HttpBlock(HttpBlock const & copy);
	~HttpBlock(void);
	HttpBlock& operator=(HttpBlock const & rhs);

	// Setters
	void	setAutoIndex(char state);
	void	setChunkedEncoding(char state);
	void	setRoot(std::string path);
	void	setErrorPages(std::map<int, std::string>);
	void	setMaxBdySize(size_type size);
	void	setKeepaliveTimeout(size_type timeout);
	void	setIndex(std::string index);
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
