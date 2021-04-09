/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpBlock.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 09:29:05 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/09 14:26:46 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpBlock.hpp"

HttpBlock::HttpBlock(void)
: _autoindex(false), _chunked_transfer_encoding(true), _root(),
_error_pages(), _client_max_body_size(10), _keepalive_timeout(0),
_index(), _servers()
{
	return;
}

HttpBlock::HttpBlock(HttpBlock const & copy)
{
	this->_autoindex = copy._autoindex;
	this->_chunked_transfer_encoding = copy._chunked_transfer_encoding;
	this->_root = copy._root;
	this->_error_pages = copy._error_pages;
	this->_client_max_body_size = copy._client_max_body_size;
	this->_keepalive_timeout = copy._keepalive_timeout;
	this->_index = copy._index;
	// this->_includes = copy._includes;
	this->_servers = copy._servers;

	return ;
}

HttpBlock::~HttpBlock(void)
{
	// delete this->_error_pages;
	// delete this->_index;
	// delete this->_servers;
	return;
}

/*HttpBlock& operator=(HttpBlock const & rhs)
{

}
*/

// Setters
void	HttpBlock::setMaxBdySize(size_type size)
{
	this->_client_max_body_size = size;
}

void	HttpBlock::setKeepaliveTimeout(size_type timeout)
{
	this->_keepalive_timeout = timeout;
}

// Getters
std::vector<ServerBlock>&	HttpBlock::getServers()
{
	return this->_servers;
}

ServerBlock&	HttpBlock::getLastServer()
{
	return this->_servers.back();
}




void HttpBlock::addServer()
{
	ServerBlock srv;
	this->_servers.push_back(srv);
}

