/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpBlock.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 09:29:05 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/14 17:36:50 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpBlock.hpp"

HttpBlock::HttpBlock(void)
: _root(DEFAULT_ROOT),
_autoindex(DEFAULT_AUTOINDEX),
_index(),
_error_pages(),
_client_max_body_size(DEFAULT_MAX_BDY_SIZE),
_keepalive_timeout(DEFAULT_KEEPALIVE_T),
_chunked_transfer_encoding(DEFAULT_CHUNKED_ENC),
_auth_basic(),
_servers() {

	this->_index.push_back(DEFAULT_INDEX);
	this->_auth_basic.push_back(DEFAULT_AUTH_BASIC);
	return ;
}

HttpBlock::HttpBlock(HttpBlock const & copy)
{
	this->_root = copy._root;
	this->_autoindex = copy._autoindex;
	this->_index = copy._index;
	this->_error_pages = copy._error_pages;
	this->_client_max_body_size = copy._client_max_body_size;
	this->_keepalive_timeout = copy._keepalive_timeout;
	this->_chunked_transfer_encoding = copy._chunked_transfer_encoding;
	this->_auth_basic = copy._auth_basic;
	this->_servers = copy._servers;

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
	void	setAutoIndex(std::string state);
	void	setChunkedEncoding(char state);
	void	setLimitExcept(std::string state);
	void	setRoot(std::string path);
	void	setErrorPages(std::map<int, std::string>);
	void	setMaxBdySize(size_type size);
	void	setKeepaliveTimeout(size_type timeout);
	void	setIndex(std::string index);
	void	setAuthBasic(std::string value);

void	HttpBlock::setRoot(std::string path)
{
	this->_root = path;
}

void	HttpBlock::setAutoIndex(std::string state)
{
	if (state == "on")
		this->_autoindex = true;
	else if (state == "off")
		this->_autoindex = false;
}

void	HttpBlock::setIndex(strVecIterator first, strVecIterator last)
{
	this->_index.assign(first, last);
}

void	HttpBlock::setLimitExcept(std::string method)
{
	this->_limit_except.push_back(method);
}

void	HttpBlock::setErrorPages(strVecIterator first, strVecIterator last, std::string& val)
{
	while (first != last)
	{
		this->_error_pages.insert(std::make_pair(atoi(first->c_str()), val));
		++first;
	}
}

void	HttpBlock::setChunkedEncoding(char state)
{
	if (state == '1')
		this->_chunked_transfer_encoding = true;
	if (state == '0')
		this->_chunked_transfer_encoding = false;
}

void HttpBlock::setMaxBdySize(size_type size)
{
	this->_client_max_body_size = size;
}
void HttpBlock::setKeepaliveTimeout(size_type timeout)
{
	this->_keepalive_timeout = timeout;
}

void	HttpBlock::setAuthBasic(std::string value)
{
	this->_auth_basic.push_back(value);
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


