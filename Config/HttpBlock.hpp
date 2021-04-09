/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpBlock.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/17 16:29:31 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/09 12:46:30 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_BLOCK_HPP
# define HTTP_BLOCK_HPP

# include <string>
# include <iostream>
# include <map>
# include <vector>
# include "ServerBlock.hpp"

# define ON		0
# define OFF	1

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

public:
	HttpBlock(void);
	HttpBlock(HttpBlock const & copy);
	~HttpBlock(void);
	HttpBlock& operator=(HttpBlock const & rhs);

	// Setters
	void	setMaxBdySize(size_type size);
	void	setKeepaliveTimeout(size_type timeout);

	// Getters
	std::vector<ServerBlock>& getServers();
	ServerBlock&	getLastServer(void);

	
	void	addServer();


};

std::ostream & operator<<(std::ostream & o, HttpBlock const & rhs);

#endif // HTTP_BLOCK_HPP
