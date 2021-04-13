/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpBlock.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 09:29:05 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/13 16:39:36 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpBlock.hpp"

HttpBlock::HttpBlock(void)
: _autoindex(DEFAULT_AUTOINDEX),
_chunked_transfer_encoding(DEFAULT_CHUNKED_ENC),
_root(DEFAULT_ROOT),
_error_pages(),
_client_max_body_size(DEFAULT_MAX_BDY_SIZE),
_keepalive_timeout(DEFAULT_KEEPALIVE_T),
_index(),
_servers(),
_auth_basic() {

	this->_index.push_back(DEFAULT_INDEX);
	this->_auth_basic.push_back(DEFAULT_AUTH_BASIC);
	return ;
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
	this->_auth_basic = copy._auth_basic;

	return ;
}

HttpBlock::~HttpBlock(void)
{
	// delete this->_error_pages;
	// delete this->_index;
	// delete this->_servers;
	return ;
}

/*HttpBlock& operator=(HttpBlock const & rhs)
{

}
*/

// Setters
void HttpBlock::setRoot(std::string path)
{
	this->_root = path;
}

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


