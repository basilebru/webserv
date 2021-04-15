/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpBlock.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 09:29:05 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/15 16:42:43 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpBlock.hpp"

HttpBlock::HttpBlock(void) :
 _root(DEFAULT_ROOT),
_autoindex(DEFAULT_AUTOINDEX),
_client_max_body_size(DEFAULT_MAX_BDY_SIZE),
_keepalive_timeout(DEFAULT_KEEPALIVE_T),
_chunked_transfer_encoding(DEFAULT_CHUNKED_ENC),
_auth_basic(DEFAULT_AUTH_BASIC) {

	this->_indexes.push_back(DEFAULT_INDEX);
	return ;
}

HttpBlock::HttpBlock(HttpBlock const & copy)
{
	this->_root = copy._root;
	this->_autoindex = copy._autoindex;
	this->_indexes = copy._indexes;
	this->_error_pages = copy._error_pages;
	this->_client_max_body_size = copy._client_max_body_size;
	this->_keepalive_timeout = copy._keepalive_timeout;
	this->_chunked_transfer_encoding = copy._chunked_transfer_encoding;
	this->_auth_basic = copy._auth_basic;
	this->_auth_basic_user_file = copy._auth_basic_user_file;
	this->_servers = copy._servers;

	return ;
}

HttpBlock::~HttpBlock(void)
{
	return ;
}

/*HttpBlock& operator=(HttpBlock const & rhs)
{
	this->_root = rhs._root;
	this->_autoindex = rhs._autoindex;
	this->_indexes = rhs._indexes;
	this->_error_pages = rhs._error_pages;
	this->_client_max_body_size = rhs._client_max_body_size;
	this->_keepalive_timeout = rhs._keepalive_timeout;
	this->_chunked_transfer_encoding = rhs._chunked_transfer_encoding;
	this->_auth_basic = rhs._auth_basic;
	this->_auth_basic_user_file = rhs._auth_basic_user_file;
	this->_servers = rhs._servers;

	return *this;
}*/


void	HttpBlock::setRoot(std::string path)
{
	this->_root = path;
}

void	HttpBlock::setAutoIndex(std::string& state)
{
	if (state == "on")
		this->_autoindex = true;
	else if (state == "off")
		this->_autoindex = false;
}

void	HttpBlock::setIndexes(strVecIterator first, strVecIterator last)
{
	this->_indexes.assign(first, last);
}

void	HttpBlock::setLimitExcept(strVecIterator first, strVecIterator last)
{
	this->_limit_except.assign(first, last);
}

void	HttpBlock::setErrorPages(strVecIterator first, strVecIterator last, std::string& val)
{
	while (first != last)
	{
		this->_error_pages.insert(std::make_pair(atoi(first->c_str()), val));
		++first;
	}
}

void HttpBlock::setMaxBdySize(size_type size)
{
	this->_client_max_body_size = size;
}
void HttpBlock::setKeepaliveTimeout(size_type timeout)
{
	this->_keepalive_timeout = timeout;
}

void	HttpBlock::setChunkedEncoding(std::string& state)
{
	if (state == "on")
		this->_chunked_transfer_encoding = true;
	if (state == "off")
		this->_chunked_transfer_encoding = false;
}

void	HttpBlock::setAuthBasic(std::string value)
{
	this->_auth_basic = value;
}

void	HttpBlock::setAuthBasicFile(std::string path)
{
	this->_auth_basic_user_file = path;
}


// Getters
const std::string&	HttpBlock::getRoot(void) const
{
	return this->_root;
}

const bool&			HttpBlock::getAutoindex(void) const
{
	return this->_autoindex;
}

const stringVec&	HttpBlock::getIndexes(void) const
{
	return this->_indexes;
}

const stringVec&	HttpBlock::getLimitExcept(void) const
{
	return this->_limit_except;
}

const errorMap&		HttpBlock::getErrorPages(void) const
{
	return this->_error_pages;
}

const size_type&	HttpBlock::getMaxBdySize(void) const
{
	return this->_client_max_body_size;
}

const size_type&	HttpBlock::getKeepaliveTime(void) const
{
	return this->_keepalive_timeout;
}

const bool&			HttpBlock::getChunkedEncoding(void) const
{
	return this->_chunked_transfer_encoding;
}

const std::string&	HttpBlock::getAuthBasic(void) const
{
	return this->_auth_basic;
}

const std::string&	HttpBlock::getAuthBasicFile(void) const
{
	return this->_auth_basic_user_file;
}


const std::vector<ServerBlock>&	HttpBlock::getServers() const
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

std::ostream & operator<<(std::ostream & o, HttpBlock const & rhs)
{

	o << "HTTP BLOCK: " << std::endl;
	o << "------------" << std::endl;

	o << "ROOT: " << rhs.getRoot() << std::endl;
	o << "AUTOINDEX: " << std::boolalpha << rhs.getAutoindex() << std::endl;

	o << "INDEXES: ";
	putVecToOstream(o, rhs.getIndexes().begin(), rhs.getIndexes().end());
	
	o << "LIMIT_EXCEPT: ";
	putVecToOstream(o, rhs.getLimitExcept().begin(), rhs.getLimitExcept().end());

	o << "ERROR PAGES: ";
	putMapToOstream(o, rhs.getErrorPages().begin(), rhs.getErrorPages().end());
	
	o << "MAX BDY SIZE: " << rhs.getMaxBdySize() << std::endl;
	o << "KEEP. TIMEOUT: " << rhs.getKeepaliveTime() << std::endl;

	o << "CHUNKED ENC.: " << std::boolalpha << rhs.getChunkedEncoding() << std::endl;
	
	o << "AUTH BASIC: " << rhs.getAuthBasic() << std::endl;
	o << "AUTH BASIC FILE: " << rhs.getAuthBasicFile() << std::endl;

	// Servers
	putVecToOstream(o, rhs.getServers().begin(), rhs.getServers().end());

	return o;
}

