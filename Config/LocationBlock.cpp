/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationBlock.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 09:40:40 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/16 19:52:02 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationBlock.hpp"

LocationBlock::LocationBlock(void) :
_autoindex(NOT_SET),
_client_max_body_size(DEFAULT_MAX_BDY_SIZE),
_keepalive_timeout(DEFAULT_KEEPALIVE_T),
_chunked_transfer_encoding(NOT_SET),
_auth_basic(DEFAULT_AUTH_BASIC),
_cgi_port(DEFAULT_CGI_PORT) {

	return;
}

LocationBlock::LocationBlock(LocationBlock const & copy)
{
	this->_path = copy._path;
	this->_root = copy._root;
	this->_autoindex = copy._autoindex;
	this->_indexes = copy._indexes;
	this->_limit_except = copy._limit_except;
	this->_error_pages = copy._error_pages;
	this->_client_max_body_size = copy._client_max_body_size;
	this->_keepalive_timeout = copy._keepalive_timeout;
	this->_chunked_transfer_encoding = copy._chunked_transfer_encoding;
	this->_auth_basic = copy._auth_basic;
	this->_auth_basic_user_file = copy._auth_basic_user_file;
	this->_cgi_pass = copy._cgi_pass;
	this->_cgi_port = copy._cgi_port;
	this->_cgi_index = copy._cgi_index;
	this->_cgi_params = copy._cgi_params;

	return ;
}

LocationBlock::~LocationBlock(void)
{
	return;
}

LocationBlock& LocationBlock::operator=(LocationBlock const & rhs)
{
	this->_path = rhs._path;
	this->_root = rhs._root;
	this->_autoindex = rhs._autoindex;
	this->_indexes = rhs._indexes;
	this->_limit_except = rhs._limit_except;
	this->_error_pages = rhs._error_pages;
	this->_client_max_body_size = rhs._client_max_body_size;
	this->_keepalive_timeout = rhs._keepalive_timeout;
	this->_chunked_transfer_encoding = rhs._chunked_transfer_encoding;
	this->_auth_basic = rhs._auth_basic;
	this->_auth_basic_user_file = rhs._auth_basic_user_file;
	this->_cgi_pass = rhs._cgi_pass;
	this->_cgi_port = rhs._cgi_port;
	this->_cgi_index = rhs._cgi_index;
	this->_cgi_params = rhs._cgi_params;

	return *this;
}

// Setters

void	LocationBlock::setPath(std::string path)
{
	this->_path = path;
}

void	LocationBlock::setRoot(std::string path)
{
	this->_root = path;
}

void	LocationBlock::setAutoIndex(std::string& state)
{
	if (state == "on")
		this->_autoindex = ON;
	else if (state == "off")
		this->_autoindex = OFF;
}

void	LocationBlock::setIndexes(strVecIterator first, strVecIterator last)
{
	this->_indexes.assign(first, last);
}

void	LocationBlock::setLimitExcept(strVecIterator first, strVecIterator last)
{
	this->_limit_except.assign(first, last);
}

void	LocationBlock::setErrorPages(strVecIterator first, strVecIterator last, std::string& val)
{
	while (first != last)
	{
		this->_error_pages.insert(std::make_pair(atoi(first->c_str()), val));
		++first;
	}
}

void LocationBlock::setMaxBdySize(size_type size)
{
	this->_client_max_body_size = size;
}

void LocationBlock::setKeepaliveTimeout(size_type timeout)
{
	this->_keepalive_timeout = timeout;

}

void	LocationBlock::setChunkedEncoding(std::string& state)
{
	if (state == "on")
		this->_chunked_transfer_encoding = ON;
	if (state == "off")
		this->_chunked_transfer_encoding = OFF;
}

void	LocationBlock::setAuthBasic(std::string value)
{
	this->_auth_basic = value;
}

void	LocationBlock::setAuthBasicFile(std::string path)
{
	this->_auth_basic_user_file = path;
}

void	LocationBlock::setCgiPass(std::string value)
{
	this->_cgi_pass = value;
}

void	LocationBlock::setCgiPort(size_type port)
{
	this->_cgi_port = port;
}

void	LocationBlock::setCgiIndex(std::string index)
{
	this->_cgi_index = index;
}

void	LocationBlock::setCgiParams(mapCgiParams params)
{
	(void)params;
	// this->_;
}

// Setters
const std::string&		LocationBlock::getPath(void) const
{
	return this->_path;
}

const std::string&		LocationBlock::getRoot(void) const
{
	return this->_root;
}

const int&		LocationBlock::getAutoindex(void) const
{
	return this->_autoindex;
}

const stringVec&	LocationBlock::getIndexes(void) const
{
	return this->_indexes;
}

const stringVec&	LocationBlock::getLimitExcept(void) const
{
	return this->_limit_except;
}

const errorMap&		LocationBlock::getErrorPages(void) const
{
	return this->_error_pages;
}

const size_type&		LocationBlock::getMaxBdySize(void) const
{
	return this->_client_max_body_size;
}

const size_type&	LocationBlock::getKeepaliveTime(void) const
{
	return this->_keepalive_timeout;
}

const int&			LocationBlock::getChunkedEncoding(void) const
{
	return this->_chunked_transfer_encoding;
}

const std::string&	LocationBlock::getAuthBasic(void) const
{
	return this->_auth_basic;
}

const std::string&	LocationBlock::getAuthBasicFile(void) const
{
	return this->_auth_basic_user_file;
}

std::ostream & operator<<(std::ostream & o, LocationBlock const & rhs)
{
	static std::string pad("    ");

	o << pad << "PATH: " << rhs.getPath() << std::endl;
	o << pad << "ROOT: " << rhs.getRoot() << std::endl;
	o << pad << "AUTOINDEX: "; putState(o, rhs.getAutoindex());

	o << pad << "INDEXES: ";
	putVecToOstream(o, rhs.getIndexes().begin(), rhs.getIndexes().end());
	
	o << pad << "LIMIT_EXCEPT: ";
	putVecToOstream(o, rhs.getLimitExcept().begin(), rhs.getLimitExcept().end());

	o << pad << "ERROR PAGES: ";
	putMapToOstream(o, rhs.getErrorPages().begin(), rhs.getErrorPages().end());	

	o << pad << "MAX BDY SIZE: " << rhs.getMaxBdySize() << std::endl;
	o << pad << "KEEP. TIMEOUT: " << rhs.getKeepaliveTime() << std::endl;

	o << pad << "CHUNKED ENC.: "; putState(o, rhs.getChunkedEncoding());
	
	o << pad << "AUTH BASIC: " << rhs.getAuthBasic() << std::endl;
	o << pad << "AUTH BASIC FILE: " << rhs.getAuthBasicFile() << std::endl;

	return o;
}